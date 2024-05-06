/*
 * the heap
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "heap.h"
#include "globals.h"
#include "collector.h"
#include "statistics.h"
#include "bistree.h"

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
    if (heap->top + sizeof(_block_header) + nbytes <= heap->limit) {
        _block_header* q = (_block_header*)(heap->top);
        q->marked = 0;
        q->size = nbytes;
        char* p = heap->top + sizeof(_block_header);
        heap->top = heap->top + sizeof(_block_header) + nbytes;
        return p;
    } else {
        switch (GC_ALGORITHM) {
            case 0: {
                if (free_blocks != NULL) {
                    _block_header* q = free_blocks;
                    q->marked = 0;
                    q->size = nbytes;
                    char* p = (char*)q + sizeof(_block_header);

                    free_blocks = q->next_free_block;
                    return p;
                }

                garbageCollections++;
                printf("my_malloc: not enough space, performing GC... (%d)\n", garbageCollections);

                char* p = my_malloc_mark_and_sweep(nbytes);
                if (PRINT_STATISTICS)
                    print_mark_sweep_statistics();
                return p;
                break;
            }
            case 1: {
                garbageCollections++;
                printf("my_malloc: not enough space, performing GC... (%d)\n", garbageCollections);

                char* p = my_malloc_mark_and_compact(nbytes);

                if (PRINT_STATISTICS)
                    print_mark_compact_statistics();

                return p;
                break;
            }
            case 2: {
                garbageCollections++;
                printf("my_malloc: not enough space, performing GC... (%d)\n", garbageCollections);

                char* p = my_malloc_copy_collect(nbytes);

                if (PRINT_STATISTICS)
                    print_copy_collect_statistics();

                return p;

                break;
            }
            default:
                printf("Something went wrong choosing a malloc() function\n");
                return NULL;
                break;
        }
    }
}

void* my_malloc_mark_and_sweep(unsigned int nbytes) {
    heap->collector();

    if (free_blocks == NULL) {
        printf("my_malloc: not enough space after GC...\n");
        return NULL;
    }

    _block_header* q = (_block_header*)free_blocks;
    q->marked = 0;
    q->size = nbytes;
    char* p = (char*)q + sizeof(_block_header);

    free_blocks = q->next_free_block;

    return p;
}

void* my_malloc_mark_and_compact(unsigned int nbytes) {
    heap->collector();

    if (heap->top + sizeof(_block_header) + nbytes > heap->limit) {
        printf("my_malloc: not enough space after GC...\n");
        return NULL;
    }

    _block_header* q = (_block_header*)heap->top;
    q->marked = 0;
    q->size = nbytes;
    char* p = heap->top + sizeof(_block_header);

    heap->top = heap->top + sizeof(_block_header) + nbytes;

    return p;
}

void* my_malloc_copy_collect(unsigned int nbytes) {
    heap->collector();

    if (heap->top + sizeof(_block_header) + nbytes > heap->limit) {
        printf("my_malloc: not enough space after GC...\n");
        return NULL;
    }

    _block_header* q = (_block_header*)heap->top;
    q->marked = 0;
    q->size = nbytes;
    char* p = heap->top + sizeof(_block_header);

    heap->top = heap->top + sizeof(_block_header) + nbytes;

    return p;
}

void add(_block_header* block) {
    if (block == NULL) return;  // Early return if block is NULL

    block->next_free_block = NULL;  // Ensure the new block points to NULL

    if (free_blocks == NULL) {
        free_blocks = block;  // Set as first block if list is empty
    } else {
        block->next_free_block = free_blocks;
        free_blocks = block;
    }
}
