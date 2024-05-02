/*
 * the heap
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "heap.h"
#include "globals.h"
#include "collector.h"

void heap_init(Heap* heap, unsigned int size) {
    heap->base = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    heap->size = size;
    heap->limit = heap->base + size;
    heap->top = heap->base;
    free_blocks = NULL;

    switch (GC_ALGORITHM) {
        case 0:
            heap->collector = mark_sweep_gc;
            break;
        case 1:
            heap->collector = mark_compact_gc;
            break;
        default:
            printf("Something went wrong when choosing a GC algoritm\n");
            exit(1);
            break;
    }
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
    } else {
        garbageCollections++;
        printf("my_malloc: not enough space, performing GC... (%d)\n", garbageCollections);
        switch (GC_ALGORITHM) {
            case 0:
                return my_malloc_mark_and_sweep(nbytes);
                break;
            case 1:
                _block_header* q = (_block_header*)my_malloc_mark_and_compact(nbytes);
                q->marked = 0;
                q->size = nbytes;
                char* p = heap->top + sizeof(_block_header);
                heap->top = heap->top + sizeof(_block_header) + nbytes;
                return p;
                break;
            default:
                printf("Something went wrong choosing a malloc() function\n");
                return NULL;
                break;
        }
    }
}

void* my_malloc_mark_and_sweep(unsigned int nbytes) {
    if (free_blocks != NULL) {
        garbageCollections--;  // I won't count this as garbage collection

        _block_header* q = free_blocks;
        q->marked = 0;
        q->size = nbytes;
        char* p = (char*)q + sizeof(_block_header);

        free_blocks = q->next_free_block;
        return p;
    } else {
        heap->collector(roots);

        if (free_blocks == NULL) {
            printf("my_malloc: not enough space after GC...\n");
            printf("Number of garbage collections: %d\n", garbageCollections);
            return NULL;
        }

        _block_header* q = (_block_header*)free_blocks;
        q->marked = 0;
        q->size = nbytes;
        char* p = (char*)q + sizeof(_block_header);

        free_blocks = q->next_free_block;

        return p;
    }
}

void* my_malloc_mark_and_compact(unsigned int nbytes) {
    printf("Heap Base: %p\n", heap->base);
    printf("Heap Top: %p\n", heap->top);
    printf("Heap Limit: %p\n", heap->limit);
    heap->collector(roots);
    printf("Heap Base: %p\n", heap->base);
    printf("Heap Top: %p\n", heap->top);
    printf("Heap Limit: %p\n", heap->limit);

    if (heap->top + sizeof(_block_header) + nbytes >= heap->limit) {
        printf("my_malloc: not enough space after GC...\n");
        printf("Number of garbage collections: %d\n", garbageCollections);
        return NULL;
    }

    return heap->top;
}

void add(_block_header* block) {
    if (block == NULL) return;  // Early return if block is NULL

    block->next_free_block = NULL;  // Ensure the new block points to NULL

    if (free_blocks == NULL) {
        free_blocks = block;  // Set as first block if list is empty
    } else {
        // O(1)
        block->next_free_block = free_blocks;
        free_blocks = block;

        // O(n)
        // _block_header* scan = free_blocks;
        // while (scan->next_free_block != NULL) {
        //     scan = scan->next_free_block;  // Traverse to the end of the list
        // }
        // scan->next_free_block = block;  // Append the new block at the end
    }
}
