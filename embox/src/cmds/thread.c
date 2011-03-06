/**
 * @file
 * @brief Prints threads statistic for Embox
 *
 * @date 2 Dec 2010
 * @author Gleb Efimov
 * @author Alina Kramar
 * @author Roman Oderov
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdio.h>
#include <assert.h>
#include <kernel/thread.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: thread [-h] [-s] [-k <thread_id>]\n");
}

static void print_stat(void) {
	struct thread *thread;
	int run = 0, wait = 0, zombie = 0;
	int total = 0;

	printf(" %3s %4s %6s\n", "Id", "Prio", "State");

	thread_foreach(thread) {
		const char *state;
		assert(thread->exist);

		switch (thread->state) {
		case THREAD_STATE_RUN:
			state = "run";
			run++;
			break;
		case THREAD_STATE_WAIT:
			state = "wait";
			wait++;
			break;
		case THREAD_STATE_ZOMBIE:
			state = "zombie";
			zombie++;
			break;
		default:
			state = "unknown";
			break;
		}
		printf(" %3d %4d %6s\n", thread->id, thread->priority, state);
	}

	total = run + wait + zombie;

	printf("Total: %d threads (%d run,  %d wait, %d zombie)\n", total, run,
			wait, zombie);
}

static void kill_thread(int thread_id) {
	struct thread *thread;
	int error;

	if (thread_id < 0) {
		printf("Invalid (negative) thread id: %d\n", thread_id);
		return;
	}

	if ((thread = thread_get_by_id(thread_id)) == NULL) {
		printf("No thread with id: %d\n", thread_id);
		return;
	}

	if (thread == idle_thread) {
		printf("Can't kill idle thread\n");
		return;
	}

	if ((error = thread_stop(thread))) {
		printf("Unable to kill thread %d: %s\n", thread_id, strerror(error));
		return;
	}

	printf("Thread %d killed\n", thread_id);
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	getopt_init();

	while ((opt = getopt(argc, argv, "hsk:")) != -1) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 's':
			print_stat();
			break;
		case 'k': {
			int thread_id;
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &thread_id))) {
				print_usage();
				break;
			}
			kill_thread(thread_id);
			break;
		}
		default:
			break;
		}
	}

	return 0;
}

