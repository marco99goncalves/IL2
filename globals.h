/*
 * globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "list.h"
#include "heap.h"

#define GET_HEADER_FROM_NODE(node) \
    (((_block_header *)(node)) - 1)

#define GET_NODE_FROM_HEADER(header) \
    (((BiTreeNode *)((header) + 1)))

#define NEXT_HEADER(header) \
    ((_block_header *)(((BiTreeNode *)((header) + 1)) + 1))

#define BLOCK_LIMIT(block) \
    (((char *)(block)) + sizeof(_block_header) + sizeof(BiTreeNode))

#define BLOCK_SIZE \
    (sizeof(_block_header) + sizeof(BiTreeNode))

#define BLOCKS_IN_HEAP \
    ((heap->limit - heap->base) / (BLOCK_SIZE))

#define GC_ALGORITHM 1

extern List *roots;
extern Heap *heap;
extern void *free_blocks;

// These are purely for debugging and printing statistics
#define PRINT_STATISTICS 1

extern int insertedNodes;
extern int removedNodes;
extern int garbageCollections;
extern int markedNodes;
extern int sweptNodes;
extern int totalInsertedNodes;
extern int totalRemovedNodes;

#endif
