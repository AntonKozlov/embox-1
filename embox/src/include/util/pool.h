/**
 * @file
 * @brief Static slab allocator
 *
 * @details
 *		Divide up memory into pools of objects, where each pool
 *		contains objects of the same size, and
 *		different pools contain objects of other sizes.
 */

#ifndef UTIL_POOL_H_
#define UTIL_POOL_H_

#include __impl_x(util/pool_impl.h)

/** cache descriptor */
typedef struct static_cache static_cache_t;

/**
 * create cache
 * @param type of objects in cache
 * @param name of cache
 * @param count of objects in cache
 */
#define POOL_DEF(type, name, count) \
	__POOL_DEF(type, name, count)

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object or NULL if pool is full
 */
extern void* static_cache_alloc(static_cache_t* cachep);

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
extern void static_cache_free(static_cache_t* cachep, void* objp);

#endif /* UTIL_POOL_H_ */
