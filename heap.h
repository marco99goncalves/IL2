/*
 * heap.h
 */

#ifndef HEAP_H
#define HEAP_H

#include "list.h"

typedef struct {
    unsigned int marked;
    unsigned int size;
} _block_header;

typedef struct {
    unsigned int size;
    char* base;
    char* top;
    char* limit;
    List* freeb;
    void (*collector)(List*, void*);
} Heap;

void heap_init(Heap* heap, unsigned int size, void (*collector)(List*, void*));

void heap_destroy(Heap* heap);

void* my_malloc(unsigned int nbytes);

void reset_freeb(Heap* heap);

#endif
