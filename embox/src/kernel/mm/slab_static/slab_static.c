/**
 *@file
 *@brief slab allocator
 * some beginning in this way
 *
 *@date 27.11.2010
 *@author Alexandr Kalmuk
 *@author Kirill Tyushev
 *@author Dmitry Zubarevich
 */

#include <string.h>
#include <kernel/mm/slab_static.h>

/**
 * simple cache initialization
 * @param cache which will be initialize
 */
static void static_cache_init(static_cache_t* cache) {
	struct list_head* elem;

	cache->obj_ptr.next = &(cache->obj_ptr);
	cache->obj_ptr.prev = &(cache->obj_ptr);
	for (int i = 0; i < cache->num; i++) {
		elem = (struct list_head*) cache->cache_begin + cache->size * i;

		/*add this free block in slab_list*/
		list_add(elem, &(cache->obj_ptr));
	}
}

void* static_cache_alloc(static_cache_t* cachep) {
	void* objp;

	if (!(cachep->hasinit)) {
		static_cache_init(cachep);
		cachep->hasinit = 1;
	}

	if (list_empty(&(cachep->obj_ptr)))
		return NULL;

	objp = cachep->obj_ptr.next;
	list_del(cachep->obj_ptr.next);

	return objp;
}

void  static_cache_free(static_cache_t* cachep, void* objp) {
	struct list_head* ptr_begin;

	if (objp == NULL)
		return;

	ptr_begin = (struct list_head*)(objp);
	list_add(ptr_begin, &(cachep->obj_ptr));
}


