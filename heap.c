/*
 * the heap
 */

#include "heap.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "collector.h"
#include "globals.h"

void heap_init(Heap* heap, unsigned int size, void (*collector)(List*)) {
    heap->base = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    heap->size = size;
    heap->limit = heap->base + size;
    heap->top = heap->base;
    heap->freeb = (List*)malloc(sizeof(List));
    list_init(heap->freeb);
    heap->collector = collector;
    return;
}

void heap_destroy(Heap* heap) {
    munmap(heap->base, heap->size);
    return;
}

void* my_malloc(unsigned int nbytes) {
    if (heap->top + sizeof(_block_header) + nbytes < heap->limit) {
        _block_header* q = (_block_header*)(heap->top);
        q->marked = 0;
        q->size = nbytes;
        char* p = heap->top + sizeof(_block_header);
        heap->top = heap->top + sizeof(_block_header) + nbytes;
        return p;
    } else if (!list_isempty(heap->freeb)) {
        // printf("my_malloc: not enough space, reusing free block...\n");
        _block_header* block = (_block_header*)heap->freeb->first->data;
        block->marked = 0;
        block->size = nbytes;

        // char* p = ((char*)block) + sizeof(_block_header);
        void* p = block_from_header(heap->freeb->first->data);
        list_removefirst(heap->freeb);
        return p;
    } else {
        nodesAdded = 0;
        nodesRemoved = 0;

        printf("my_malloc: not enough space, performing GC...\n");
        reset_freeb(heap);
        heap->collector(roots);
        // 0x7f4969716b00
        printf("FInishes GC :)\n");
        if (list_isempty(heap->freeb)) {
            printf("my_malloc: not enough space after GC...\n");
            exit(1);
        }
        _block_header* block = heap->freeb->first->data;
        block->marked = 0;
        block->size = nbytes;

        printf("After list check\n");
        // char* p = ((char*)block) + sizeof(_block_header);
        void* p = block_from_header(heap->freeb->first->data);
        return p;
    }
}

void reset_freeb(Heap* heap) {
    // printf("teste\n");
    free_list(heap->freeb);
    // printf("after free list\n");
    free(heap->freeb);
    // printf("after free\n");
    heap->freeb = (List*)malloc(sizeof(List));
    // printf("after malloc");
    list_init(heap->freeb);
}