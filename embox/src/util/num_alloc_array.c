/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.06.2012
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <util/num_alloc.h>

util_num_alloc_t *util_num_alloc_init(void *space, int n) {
	util_num_alloc_t *self = (util_num_alloc_t *) space;

	memset(space, 0, UTIL_NUM_ALLOC_CALC(n));

	self->n = n;

	return self;
}

int util_num_alloc(util_num_alloc_t *num_alloc) {
	int n = util_num_alloc_next_alloc(num_alloc);

	if (n < 0) {
		return n;
	}

	num_alloc->mark_table[n] = 1;

	return n;
}

void util_num_free(util_num_alloc_t *num_alloc, int n) {
	assert(num_alloc->mark_table[n]);

	num_alloc->mark_table[n] = 0;
}

int util_num_alloc_next_mark(util_num_alloc_t *num_alloc, int i, char mark) {
	int m;
	mark = mark ? 1 : 0;

	for (int j = i; j < num_alloc->n; j++) {
		m = num_alloc->mark_table[j] ? 1 : 0;
		if (mark == m) {
			return j;
		}
	}

	return -ENOENT;
}

int util_num_alloc_next_alloc(util_num_alloc_t *num_alloc) {
	int n = util_num_alloc_next_mark(num_alloc, 0, 0);

	if (n == -ENOENT) {
		return -ENOMEM;
	} 
	
	return n;
}

int util_num_alloc_specified(util_num_alloc_t *num_alloc, int n) {
	if (util_num_alloc_test(num_alloc, n)) {
		return -ENOMEM;
	}

	num_alloc->mark_table[n] = 1;

	return n;
}

int util_num_alloc_test(util_num_alloc_t *num_alloc, int n) {
	return (int) num_alloc->mark_table[n];
}
