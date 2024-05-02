/*
 * heap.h
 */

#ifndef HEAP_H
#define HEAP_H

#include "list.h"

/*
   0 - Mark and Sweep
   1 - Mark and Compact
*/

#define GC_ALGORITHM 1

typedef struct {
    unsigned int marked;
    unsigned int size;
    void* next_free_block;
} _block_header;

typedef struct {
    unsigned int size;
    char* base;
    char* top;
    char* limit;
    void (*collector)(List*);
} Heap;

void heap_init(Heap* heap, unsigned int size);

void heap_destroy(Heap* heap);

void* my_malloc(unsigned int nbytes);
void* my_malloc_mark_and_sweep(unsigned int nbytes);
void* my_malloc_mark_and_compact(unsigned int nbytes);

void add(_block_header* block);

#endif
