/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_NODE_H_
#define FS_NODE_H_

#include <fs/file_system.h>
#include <util/tree.h>

#include <module/embox/fs/core.h>
#include <framework/mod/options.h>


#define MAX_LENGTH_FILE_NAME  OPTION_MODULE_GET(embox__fs__core,NUMBER,file_name_length)
#define MAX_LENGTH_PATH_NAME  OPTION_MODULE_GET(embox__fs__core,NUMBER,path_length)



#define NODE_TYPE_FILE       0x01
#define NODE_TYPE_DIRECTORY  0x10
#define NODE_TYPE_SPECIAL    0x20

typedef struct node {
	const char            name[MAX_LENGTH_FILE_NAME];
	int                   type;  /* FILE, DIRECTORY, DEVICE, LINK ... */
	int                   mode;
	struct tree_link      tree_link;
	void                 *nas;
} node_t;

struct node_info {
	size_t        size;
	unsigned int  mtime;
};

struct node_fi {
	struct node_info ni;
	void  *privdata;
};

typedef struct nas {
	struct node          *node;
	struct filesystem    *fs;
	struct node_fi       *fi;
} nas_t;



extern node_t *node_alloc(const char *name);
extern void node_free(node_t *node);


static inline int node_is_block_dev(struct node *node) {
	return 1;
}

static inline int node_is_directory(struct node *node) {
	return node->type & NODE_TYPE_DIRECTORY;
}

static inline int node_is_file(struct node *node) {
	return node->type & NODE_TYPE_FILE;
}

#endif /* FS_NODE_H_ */
