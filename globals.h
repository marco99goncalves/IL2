/*
 * globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "list.h"
#include "heap.h"

#define GET_HEADER_FROM_NODE(node) \
    (_block_header *)((_block_header *)(node)) - 1;

#define GET_NODE_FROM_HEADER(header) \
    (((BiTreeNode *)((header) + 1)));

#define NEXT_HEADER(header) \
    (_block_header *)(((BiTreeNode *)((header) + 1)) + 1);

#define BLOCK_LIMIT(block) \
    (((char *)(block)) + sizeof(_block_header) + sizeof(BiTreeNode))

extern List *roots;
extern Heap *heap;
extern void *free_blocks;

extern int insertedNodes;
extern int removedNodes;
extern int garbageCollections;

#endif
