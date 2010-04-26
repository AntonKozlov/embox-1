/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 * Implementation look at src/kernel/thread.c.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <kernel/thread.h>

/**
 * Initializes threads:  describes idle_thread.
 */
void scheduler_init(void);

/**
 * Start working with threads.
 */
void scheduler_start(void);

/**
 * Is called after entering a regular critical section.
 * Increases preemption_count.
 */
void scheduler_lock(void);

/**
 * Is called after escaping last critical section.
 * Decreases preemption_count. If the latter one becomes zero,
 * calls scheduler_dispatch.
 */
void scheduler_unlock(void);

/**
 * Changes executable thread.
 */
void scheduler_dispatch(void);

/**
 * Is regularly called to show that current thread to be changed.
 *
 * @param id nothing significant
 */
void scheduler_tick(uint32_t id);

/**
 * Adds thread into the list of executable threads.
 *
 * @param added_thread thread to be added into the list
 */
void scheduler_add(struct thread *added_thread);

/**
 * Removes a thread from the list of executable threads.
 * @param removed_thread deleted thread
 * @retval 0 if thread was successfully removed.
 * @retval -EINVAL if @c removed_thread is NULL or &idle_thread.
 */
int scheduler_remove(struct thread *removed_thread);

#endif /* SCHEDULER_H_ */
