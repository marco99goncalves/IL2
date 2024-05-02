/*
 * globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "heap.h"
#include "list.h"

#define header_from_block(block) ((_block_header*)(block)-1)
#define block_from_header(header) ((_block_header*)(header) + 1)
#define next_header(header) ((_block_header*)((BiTreeNode*)(header + 1) + 1))

extern List* roots;
extern Heap* heap;

extern int nodesAdded;
extern int nodesRemoved;

#endif
