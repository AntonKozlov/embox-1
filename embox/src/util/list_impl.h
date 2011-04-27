/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date Feb 27, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_IMPL_H_
#define UTIL_LIST_IMPL_H_

#include <util/structof.h>

struct __list_link {
	struct __list_link *next, *prev;
};

#define __LIST_LINK_INIT__(__link) \
	{                     \
		.next = (__link), \
		.prev = (__link), \
	}

static void __list_link_init(struct __list_link *link);
static int __list_link_alone(struct __list_link *link);
static void __list_bind(struct __list_link *prev, struct __list_link *next);
static void __list_insert_chain(struct __list_link *first,
		struct __list_link *last, struct __list_link *prev,
		struct __list_link *next);
static void __list_insert_link(struct __list_link *link,
		struct __list_link *prev, struct __list_link *next);

#ifdef CONFIG_LIST_NDEBUG
# include "list_ndebug.h"
#else
# include "list_debug.h"
#endif

/* Most of macros are defined through a corresponding _link method.
 * Implementation may also provide __list_check() macro which have to return
 * its argument possibly performing an extra assertions about the value of the
 * argument. */

#ifndef __list_check
#define __list_check(expr) (expr)
#endif

static inline void __list_bind(struct __list_link *prev,
		struct __list_link *next) {
	__list_check(next)->prev = prev;
	__list_check(prev)->next = next;
}

static inline int __list_link_alone(struct __list_link *link) {
	return link == __list_check(link)->next;
}

static inline void __list_link_init(struct __list_link *link) {
	__list_bind(link, link);
}

static inline void __list_insert_chain(struct __list_link *first,
		struct __list_link *last, struct __list_link *prev,
		struct __list_link *next) {
	__list_bind(prev, first);
	__list_bind(last, next);
}

static inline void __list_insert_link(struct __list_link *link,
		struct __list_link *prev, struct __list_link *next) {
	__list_insert_chain(link, link, prev, next);
}

/* This is simply defined through structof macro. */
#define __list_link_element(link, type, m_link) \
	structof(__list_check(link), type, m_link)

#define __list_alone(element, m_link) \
	list_alone_link(&__list_check(element)->m_link)

#define __list_foreach_link(link, list) \
	__list_foreach__(link, list, __list_foreach_link_cast_assign, /* unused */)
#define __list_foreach_link_cast_assign(_iter, link, ignored) \
	link = structof(_iter, struct list_link, l)

#define __list_foreach(element, list, m_link) \
	__list_foreach__(element, list, __list_foreach_cast_assign, m_link)
#define __list_foreach_cast_assign(_iter, element, member) \
	element = structof(structof(_iter, struct list_link, l), \
			typeof(*element), member)

#define __list_foreach__(node, list, iter_cast, cast_arg) \
	__list_foreach_guarded(node, list, iter_cast, cast_arg, \
			MACRO_GUARD(__list_foreach_iter), \
			MACRO_GUARD(__list_foreach_head), \
			MACRO_GUARD(__list_foreach_next))

#define __list_foreach_guarded(node, list, iter_cast_assign, cast_arg, \
		_iter, _head, _next) \
	for(struct __list_link *_head = &__list_check(list)->l, \
				*_iter = (_head)->next, *_next = _iter->next; \
			_iter != _head && (iter_cast_assign(_iter, node, cast_arg)); \
			_iter = _next, _next = _iter->next)

/**
 * Casts the given @a link out to its container in case that it is not @c NULL.
 *
 * @param link
 *   An expression of type <em>struct list_link *</em> being casted.
 * @param type
 *   The type of the container.
 * @param m_link
 *   The name of the member within the struct
 * @return
 *   The cast result if the @a link is not @c NULL.
 * @retval NULL
 *   If the @a link is @c NULL.
 */
#define __list_link_safe_cast(link, type, m_link) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ ? list_link_element(__list_link__, type, m_link) : NULL;\
	})

#define __list_first(list, type, m_link) \
	__list_link_safe_cast(list_first_link(list), type, m_link)

#define __list_last(list, type, m_link) \
	__list_link_safe_cast(list_last_link(list), type, m_link)

#define __list_add_first(element, list, m_link) \
	list_add_first_link(&__list_check(element)->m_link, list)

#define __list_add_last(element, list, m_link) \
	list_add_last_link(&__list_check(element)->m_link, list)

#define __list_insert_before(element, list_element, m_link) \
	list_insert_before_link(&__list_check(element)->m_link, \
			&__list_check(list_element)->m_link)

#define __list_insert_after(element, list_element, m_link) \
	list_insert_after_link(&__list_check(element)->m_link, \
			&__list_check(list_element)->m_link)

#define __list_bulk_insert_before(from_list, before_element, m_link) \
	list_bulk_insert_before_link(from_list, \
			&__list_check(before_element)->m_link)

#define __list_bulk_insert_after(from_list, after_element, m_link) \
	list_bulk_insert_after_link(from_list, \
			&__list_check(after_element)->m_link)

#define __list_remove_first(list, type, m_link) \
	__list_link_safe_cast(list_remove_first_link(list), type, m_link)

#define __list_remove_last(list, type, m_link) \
	__list_link_safe_cast(list_remove_last_link(list), type, m_link)

#define __list_remove(element, m_link) \
	list_remove_link(&__list_check(element)->m_link)

#endif /* UTIL_LIST_IMPL_H_ */
