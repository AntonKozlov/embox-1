/**
 * @file
 * @brief Preemptive priority scheduler based on multilevel queue.
 *
 * @date 22.04.2010
 * @author Darya Dzendzik
 *          - Initial implementation
 * @author Alina Kramar
 *          - Fixing switch logic
 *          - Interface change
 *          - Introducing auxiliary functions for operating with queue
 *
 * @see tests/kernel/thread/sched_policy/priority_based_test.c
 */

#include <assert.h>
#include <stdbool.h>

#include <lib/list.h>
#include <util/array.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched_policy.h>

/**
 * Structure priority in list.
 */
struct run_thread_list {
	/** Link within sorted priorities list. */
	struct list_head priority_link;

	/** List of threads with given priority. */
	struct list_head thread_list;
};

/**
 * Thread that runs now.
 */
static struct thread *current_thread;

/**
 * Array of thread list ordered by priority
 * (index is priority of each thread contained in list).
 */
static struct run_thread_list priorities[THREAD_PRIORITY_TOTAL];

/**
 * Priority queue that contains queue of thread. Main queue is ordered by thread
 * priority. Internal queue is sorted by time of thread adding. Doesn't contain
 * the current thread. Head of run_queue is a thread which most appropriate for
 * execution of all of existing thread.
 * Contains idle_thread (last).
 */
static struct list_head run_queue;

static void run_insert_priority(struct run_thread_list *priority);

/* Method for more comfortable work with the queue. */

/**
 * Returns a thread from the queue top without removing it from there.
 * @return
 *   Top of queue.
 */
static struct thread *run_peek(void) {
	struct run_thread_list *priority;

	if (list_empty(&run_queue)){
		return NULL;
	}

	priority = list_entry(run_queue.next, struct run_thread_list, priority_link);
	if (list_empty(&priority->thread_list)){
		return NULL;
	}

	return list_entry(priority->thread_list.next, struct thread, sched_list);
}

/**
 * Returns a thread from the queue top and then removes it from there.
 * @return
 *   Queue top.
 */
static struct thread *run_dequeue(void) {
	struct thread *thread = run_peek();

	if(!thread) {
		return NULL;
	}

	list_del_init(&thread->sched_list);
	return thread;
}

/**
 * Adds thread to the end of the run_queue without changing the ordering.
 * @param thread
 *   Thread to add.
 */
static void run_enqueue(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;
	if (list_empty(&priority->priority_link)){
		run_insert_priority(priority);
	}
	list_add_tail(&thread->sched_list, &priority->thread_list);
}

/**
 * Adds thread to the head of the run_queue without changing the ordering.
 * @param thread
 *   Thread to add.
 */
static void run_push(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;
	if (list_empty(&priority->priority_link)){
		run_insert_priority(priority);
	}
	list_add(&thread->sched_list, &priority->thread_list);
}

/**
 * Insert @a priority to run_queue if threads with given priority don't exist.
 * Doesn't change the right order.
 * @param priority
 *   Links to thread and priority queues.
 */
static void run_insert_priority(struct run_thread_list *priority) {
	struct run_thread_list *next_priority;

	assert(list_empty(&priority->priority_link));

	if (list_empty(&run_queue)) {
		list_add(&priority->priority_link, &run_queue);
		return;
	}

	list_for_each_entry(next_priority, &run_queue, priority_link) {
		if (next_priority > priority) {
			break;
		}
	}

	list_add_tail(&priority->priority_link, &next_priority->priority_link);
}

struct thread *sched_policy_current(void) {
	return current_thread;
}

bool sched_policy_add(struct thread *thread) {
	run_enqueue(thread);
	return (thread->priority < current_thread->priority);
}

struct thread *sched_policy_switch(struct thread *prev_thread) {
	struct run_thread_list *priority = priorities + prev_thread->priority;
	struct thread *next;

	assert(current_thread != NULL);

	if (current_thread->state == THREAD_STATE_RUN) {
		if (!(next = run_peek())) {
			return current_thread;
		}
		if (current_thread->priority > next->priority) {
			/* Preemption */
			run_push(current_thread);
		} else {
			/* Quantum expiration */
			run_enqueue(current_thread);
		}
	} else {
		if (list_empty(&priority->thread_list)) {
			list_del_init(&priority->priority_link);
		}
	}

	if (!(next = run_dequeue())) {
		return current_thread;
	}

	return (current_thread = next);
}

bool sched_policy_remove(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;

	if (thread == current_thread) {
		current_thread->state = THREAD_STATE_ZOMBIE;
		return true;
	}

	list_del_init(&thread->sched_list);

	if (list_empty(&priority->thread_list)) {
		/* Remove link on list of threads with given priority */
		list_del_init(&priority->priority_link);
	}

	return false;
}

void sched_policy_start(void) {
	/* Nothing to do. */
}

void sched_policy_stop(void) {
	/* Nothing to do. */
}

/**
 * Initializes priority based policy.
 */
void sched_policy_init(struct thread *current, struct thread *idle) {
	struct run_thread_list *ptr;

	assert(current != NULL);

	INIT_LIST_HEAD(&run_queue);
	array_static_foreach_ptr(ptr, priorities) {
		INIT_LIST_HEAD(&(ptr->thread_list));
		INIT_LIST_HEAD(&(ptr->priority_link));
	}

	current_thread = current;
	run_insert_priority(priorities + current->priority);

	sched_policy_add(idle);
}
