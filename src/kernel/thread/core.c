/**
 * @file
 * @brief Thread management API implementation.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Alina Kramar
 *          - Thread control block memory allocation
 *          - Move state management code into the scheduler
 *          - Suspend/resume logic
 * @author Eldar Abusalimov
 *          - Reviewing and simplifying threads API
 *          - Stack allocation
 * @author Anton Kozlov
 *          - Tasks binding
 *
 * @see tests/kernel/thread/core_test.c
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <embox/unit.h>

#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_alloc.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/thread/thread_priority.h>
#include <kernel/thread/priority_priv.h>
#include <kernel/sched/sched_priority.h>
#include <kernel/sched/schedee.h>
#include <kernel/sched/current.h>
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

#include <kernel/panic.h>

#include <hal/context.h>
#include <util/err.h>

extern void thread_switch(struct thread *prev, struct thread *next);
extern void thread_ack_switched(void);

static int id_counter = 0; // TODO make it an indexator

/**
 * Wrapper for thread start routine.
 * Called from sched_switch() function with interrupts off.
 */
static void __attribute__((noreturn)) thread_trampoline(void) {
	struct thread *current = thread_self();
	void *res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK), "0x%x", (uint32_t)__critical_count);

	thread_ack_switched();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	/* execute user function handler */
	res = current->schedee.run(current->schedee.run_arg);
	thread_exit(res);
	/* NOTREACHED */
}

static sched_priority_t thread_priority_by_flags(unsigned int flags) {
	sched_priority_t priority;

	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		priority = thread_priority_get(thread_self());
	} else {
		priority = THREAD_PRIORITY_DEFAULT;
	}

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (priority > THREAD_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority < THREAD_PRIORITY_HIGH)) {
		priority++;
	}

	return priority;
}

struct thread *thread_create(unsigned int flags, void *(*run)(void *), void *arg) {
	struct thread *t;
	sched_priority_t priority;

	/* check mutually exclusive flags */
	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (flags & THREAD_FLAG_PRIORITY_HIGHER)) {
		return err_ptr(EINVAL);
	}

	if((flags & THREAD_FLAG_NOTASK) && !(flags & THREAD_FLAG_SUSPENDED)) {
		return err_ptr(EINVAL);
	}

	/* check correct executive function */
	if (!run) {
		return err_ptr(EINVAL);
	}

	/* calculate current thread priority. It can be change later with
	 * thread_set_priority () function
	 */
	priority = thread_priority_by_flags(flags);

	/* below we will work with thread's instances and therefore we need to
	 * lock scheduler (disable scheduling) to our structures is not be
	 * corrupted
	 */
	sched_lock();
	{
		/* allocate memory */
		if (!(t = thread_alloc())) {
			t = err_ptr(ENOMEM);
			goto out_unlock;
		}

		/* initialize internal thread structure */
		thread_init(t, priority, run, arg);

		/* link with task if needed */
		if (!(flags & THREAD_FLAG_NOTASK)) {
			task_thread_register(task_self(), t);
		}

		thread_cancel_init(t);

		if (!(flags & THREAD_FLAG_SUSPENDED)) {
			thread_launch(t);
		}

		if (flags & THREAD_FLAG_DETACHED) {
			thread_detach(t);
		}

	}
out_unlock:
	sched_unlock();

	return t;
}

static struct schedee *thread_process(struct schedee *prev, struct schedee *next) {
	struct thread *next_t, *prev_t;
	next_t = mcast_out(next, struct thread, schedee);
	prev_t = mcast_out(prev, struct thread, schedee);

	schedee_set_current(next);

	/* Threads context switch */
	if (prev != next) {
		thread_switch(prev_t, next_t);
	}

	ipl_enable();

	if (!prev_t->siglock) {
		thread_signal_handle();
	}

	return &thread_self()->schedee;
}

struct thread *thread_self(void) {
	struct schedee *schedee = schedee_get_current();
	assert(schedee->process == thread_process, "thread_self is about to return not-thread");
	return mcast_out(schedee, struct thread, schedee);;
}

void thread_init(struct thread *t, sched_priority_t priority,
		void *(*run)(void *), void *arg) {

	assert(t);
	assert(run);
	assert(thread_stack_get(t));
	assert(thread_stack_get_size(t));

	t->id = id_counter++; /* setup thread ID */

	dlist_head_init(&t->thread_link); /* default unlink value */

	t->task = NULL;

	t->critical_count = __CRITICAL_COUNT(CRITICAL_SCHED_LOCK);
	t->siglock = 0;

	t->state = TS_INIT;

	if (thread_local_alloc(t, MODOPS_THREAD_KEY_QUANTITY)) {
		panic("can't initialize thread_local");
	}

	t->joining = NULL;

	/* cpu context init */
	/* setup stack pointer to the top of allocated memory
	 * The structure of kernel thread stack follow:
	 * +++++++++++++++ top
	 *                  |
	 *                  v
	 * the thread structure
	 * xxxxxxx
	 * the end
	 * +++++++++++++++ bottom (t->stack - allocated memory for the stack)
	 */
	context_init(&t->context, CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE, 
			thread_trampoline, thread_stack_get(t) + thread_stack_get_size(t));

	sigstate_init(&t->sigstate);

	schedee_init(&t->schedee, priority, thread_process, run, arg);

	/* initialize everthing else */
	thread_wait_init(&t->thread_wait);
}

struct thread *thread_init_stack(void *stack, size_t stack_sz,
	       	sched_priority_t priority, void *(*run)(void *), void *arg) {
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	thread_stack_init(thread, stack_sz);

	/* General initialization and task setting up */
	thread_init(thread, priority, run, arg);

	return thread;

}
void thread_delete(struct thread *t) {
	static struct thread *zombie = NULL;

	assert(t);
	assert(t->state & TS_EXITED);

	task_thread_unregister(t->task, t);
	thread_local_free(t);

	if (zombie) {
		thread_free(zombie);
	}

	if (t != thread_self()) {
		assert(!t->schedee.active);
		assert(!t->schedee.ready);
		thread_free(t);
		zombie = NULL;
	} else {
		zombie = t;
	}
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct task *task = task_self();
	struct thread *joining;

	/* We can free only not main threads */
	if (current == task_get_main(task)) {
		/* We are last thread. */
		task_exit(ret);
		/* NOTREACHED */
	}

	sched_lock();

	// sched_finish(current);
	current->schedee.waiting = true;
	current->state |= TS_EXITED;

	/* Wake up a joining thread (if any).
	 * Note that joining and run_ret are both in a union. */
	joining = current->joining;
	current->run_ret = ret;
	if (joining) {
		sched_wakeup(&joining->schedee);
	}

	if (current->state & TS_DETACHED)
		/* No one references this thread anymore. Time to delete it. */
		thread_delete(current);

	schedule();

	/* NOTREACHED */
	sched_unlock();  /* just to be honest */
	panic("Returning from thread_exit()");
}

int thread_join(struct thread *t, void **p_ret) {
	struct thread *current = thread_self();
	int ret = 0;

	assert(t);

	if (t == current)
		return -EDEADLK;

	sched_lock();
	{
		assert(!(t->state & TS_DETACHED));

		if (!(t->state & TS_EXITED)) {
			assert(!t->joining);
			t->joining = current;

			ret = SCHED_WAIT(t->state & TS_EXITED);
			if (ret) {
				goto out;
			}
		}

		if (p_ret)
			*p_ret = t->run_ret;

		thread_delete(t);
	}
out:
	sched_unlock();

	return ret < 0 ? ret : 0;
}

int thread_detach(struct thread *t) {
	assert(t);

	sched_lock();
	{
		assert(!(t->state & TS_DETACHED));

		if (!(t->state & TS_EXITED)) {
			/* The target will free itself upon finishing. */
			assert(!t->joining);
			t->state |= TS_DETACHED;
		}
		else
			/* The target thread has finished, free it here. */
			thread_delete(t);
	}
	sched_unlock();

	return 0;
}

int thread_launch(struct thread *t) {
	int ret;

	sched_lock();
	{
		if (t->state & TS_EXITED) {
			ret = -EINVAL;
		}
		else {
			ret = sched_wakeup(&t->schedee) ? 0 : -EINVAL;
		}
	}
	sched_unlock();

	return ret;
}

int thread_terminate(struct thread *t) {
	assert(t);

	sched_lock();
	{
		// sched_finish(t);
		// assert(0, "NIY");
		// thread_delete(t);
		sched_freeze(&t->schedee);

		t->state |= TS_EXITED;

		// XXX prevent scheduler to add thread in runq
		if (t == thread_self()) {
			t->schedee.waiting = true;
		}
	}
	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_post_switch();
}

int thread_set_priority(struct thread *t, sched_priority_t new_priority) {
	// sched_priority_t prior;
	assert(t);

	if ((new_priority < THREAD_PRIORITY_MIN)
			|| (new_priority > THREAD_PRIORITY_MAX)) {
		return -EINVAL;
	}

	// prior = sched_priority_thread(t->task->priority, thread_priority_get(t));
	// if(new_priority != prior) {
	// 	prior = sched_priority_full(t->task->priority, new_priority);
	// 	sched_change_priority(t, prior);
	// }

 	sched_change_priority(&t->schedee, new_priority);


	return 0;
}

sched_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return thread_priority_get(t);
}

clock_t thread_get_running_time(struct thread *t) {
	clock_t running;

	sched_lock();
	{
		running = sched_timing_get(&t->schedee);
	}
	sched_unlock();

	return running;
}

void thread_set_run_arg(struct thread *t, void *run_arg) {
	assert(t->state == TS_INIT);
	t->schedee.run_arg = run_arg;
}