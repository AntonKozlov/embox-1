/**
 * @file
 * @brief
 *
 * @date 27.11.10
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <embox/unit.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/pipe_manager.h>
#include <kernel/thread/sched.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

EMBOX_TEST(run);

static char first_stack[THREAD_STACK_SIZE];
static char second_stack[THREAD_STACK_SIZE];
static char third_stack[THREAD_STACK_SIZE];

static void first_run(void) {
	TRACE("\n First thread: \n");
	pipe_write(0, 'k');
	pipe_write(0, 'z');
	thread_yield();
}

static void second_run(void) {
	TRACE("\n Second thread: \n");
	pipe_write(0, 'm');
	char tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);

	pipe_write(0, 's');
	pipe_write(0, 't');
	pipe_write(0, 'a');
	pipe_write(0, 'r');
	pipe_write(0, 't');
	thread_yield();
}

static void third_run(void) {
	TRACE("\n Third thread: \n");
	char tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
}

static int run(void) {
	struct thread t_first, t_second, t_third;

	thread_init(&t_third, third_run, third_stack + THREAD_STACK_SIZE);
	thread_init(&t_second, second_run, second_stack + THREAD_STACK_SIZE);
	thread_init(&t_first, first_run, first_stack + THREAD_STACK_SIZE);

	thread_start(&t_first);
	thread_start(&t_third);
	thread_start(&t_second);

	pipe_create(); // pipe #0
	pipe_create(); // pipe #1

	sched_start();

	thread_join(&t_first);
	thread_join(&t_second);
	thread_join(&t_third);

	sched_stop();

	return 0;
}
