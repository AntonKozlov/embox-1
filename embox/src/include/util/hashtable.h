/**
 * @file
 * @brief interface to hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_HASHTABLE_H_
#define UTIL_HASHTABLE_H_

#include <types.h>

struct hashtable;

/** Hash function delegate. */
typedef size_t (*get_hash_ft) (void *key);

/** compare function delegate. */
typedef int (*ht_cmp_ft) (void *key1, void *key2);

/**
 * Create hashtable
 * @param table_size - array size for hash table
 * @param key_size - size of key type
 * @param value_size - size of value type
 * @return hashtable
 */
extern struct hashtable *hashtable_create(size_t table_size, get_hash_ft get_hash, ht_cmp_ft cmp);

/**
 * Delete all elements from hashtable and free hashtable structure memory
 *
 * @param hash_tab - hashtable
 *
 */
extern void hashtable_destroy(struct hashtable *ht);


/**
 * Insert element into hashtable
 * @param hash_tab - hashtable into which will be inserted element
 * @param key - identifier of data
 * @param hash - hash function
 * @param compare - compare function
 */
extern void hashtable_put(struct hashtable *ht, void *key, void *value);

/**
 * Search of data in hashtable by key
 * @param hash_tab - hashtable at which will be found element
 * @param key - identifier for data of element
 * @param hash - hash function
 * @param compare - compare function
 */
extern void *hashtable_get(struct hashtable *ht, void* key);

/**
 * Delete element from hashtable
 * @param hash_tab - hashtable from which will be deleted element
 * @param key - identifier of data in element
 * @param hash - hash function
 * @param compare - compare function
 */
extern void hashtable_del(struct hashtable *ht, void *key, void *value);


#endif /* UTIL_HASHTABLE_H_ */
