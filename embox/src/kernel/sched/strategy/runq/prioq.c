/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <kernel/schedee/schedee.h>
#include <kernel/schedee/schedee_priority.h>
#include <kernel/sched/sched_strategy.h>

#include <util/priolist.h>

void runq_item_init(runq_item_t *runq_link) {
	priolist_link_init(runq_link);
}

void runq_init(runq_t *queue) {
	priolist_init(queue);
}

void runq_insert(runq_t *queue, struct schedee *s) {
	s->sched_attr.runq_link.prio = -schedee_priority_get(s);
	priolist_add(&s->sched_attr.runq_link, queue);
}

void runq_remove(runq_t *queue, struct schedee *s) {
	priolist_del(&s->sched_attr.runq_link, queue);
}

struct schedee *runq_extract(runq_t *queue) {
	runq_item_t *first = priolist_first(queue);
	struct schedee *result;

	priolist_del(first, queue);
	result = mcast_out(first, struct schedee, sched_attr.runq_link);

	return result;
}
