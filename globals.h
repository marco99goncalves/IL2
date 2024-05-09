/*
 * globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "list.h"
#include "heap.h"

#define NORMAL "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENT "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

#define MARK_AND_SWEEP 0
#define MARK_AND_COMPACT 1
#define COPY_COLLECT 2
#define GENERATIONAL 3

#define GET_HEADER_FROM_NODE(node) \
    (((_block_header *)(node)) - 1)

#define GET_NODE_FROM_HEADER(header) \
    (((BiTreeNode *)((header) + 1)))

#define NEXT_HEADER(header) \
    (_block_header *)(((BiTreeNode *)(((_block_header *)(header)) + 1)) + 1)

#define BLOCK_SIZE \
    (sizeof(_block_header) + sizeof(BiTreeNode))

#define BLOCKS_IN_HEAP \
    ((heap->limit - heap->base) / (BLOCK_SIZE))

extern List *roots;
extern Heap *heap;
extern void *free_blocks;
extern int GC_ALGORITHM;

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
