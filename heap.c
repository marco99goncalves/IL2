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

void checkNodes2(List* roots);
void checkTree2(BiTreeNode* node);

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

void checkNodes2(List* roots) {
    ListNode* node = roots->first;
    int t = 0;
    while (node != NULL) {
        BisTree* tree = (BisTree*)node->data;
        // printf("Checking tree %d\n", t++);
        checkTree2(tree->root);
        node = node->next;
    }
}

void checkTree2(BiTreeNode* node) {
    if (node == NULL)
        return;

    _block_header* header = GET_HEADER_FROM_NODE(node);
    // TODO: REMOVE
    if (header->marked == 1) {
        printf("SUS0\n");
        printf("header: %p\n", header);
        printf("SANITY CHECK\n");
        exit(1);
    }

    if (node < heap->base || node > heap->limit) {
        printf("SUS1\n");
        printf("node: %p\n", node);
        printf("heap->base: %p\n", heap->base);
        printf("heap->limit: %p\n", heap->limit);
        exit(1);
    }

    if ((((((char*)node) - sizeof(_block_header)) - (heap->base)) % (BLOCK_SIZE)) != 0) {
        printf("SUS2\n");
        printf("node: %p\n", (char*)node - sizeof(_block_header));
        printf("%ld\n", ((((char*)node) - sizeof(_block_header)) - (heap->base)) % (BLOCK_SIZE));
        exit(1);
    }

    checkTree2(node->left);
    checkTree2(node->right);
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
        garbageCollections++;
        printf("my_malloc: not enough space, performing GC... (%d)\n", garbageCollections);
        switch (GC_ALGORITHM) {
            case 0: {
                if (free_blocks != NULL) {
                    garbageCollections--;  // I won't count this as garbage collection

                    _block_header* q = free_blocks;
                    q->marked = 0;
                    q->size = nbytes;
                    char* p = (char*)q + sizeof(_block_header);

                    free_blocks = q->next_free_block;
                    return p;
                }

                char* p = my_malloc_mark_and_sweep(nbytes);
                if (PRINT_STATISTICS)
                    print_mark_sweep_statistics();
                return p;
                break;
            }
            case 1: {
                _block_header* q = (_block_header*)my_malloc_mark_and_compact(nbytes);
                // char* p = get_block_and_advance_heap_top(&q, nbytes);

                q = (_block_header*)(heap->top);
                q->marked = 0;
                q->size = nbytes;
                char* p = heap->top + sizeof(_block_header);
                heap->top = heap->top + sizeof(_block_header) + nbytes;
                printf("q: %p\n", q);

                printf("p: %p\n", p);
                printf("heap->top: %p\n", heap->top);
                insertedNodes = 0;
                removedNodes = 0;

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

void* get_block_and_advance_heap_top(_block_header** block, unsigned int nbytes) {
    (*block)->marked = 0;
    (*block)->size = nbytes;
    char* p = (char*)(*block) + sizeof(_block_header);
    heap->top = (char*)NEXT_HEADER((_block_header*)heap->top);
    return p;
}

void* my_malloc_mark_and_sweep(unsigned int nbytes) {
    heap->collector();

    if (free_blocks == NULL) {
        printf("my_malloc: not enough space after GC...\n");
        printf("Total number of garbage collections: %d\n", garbageCollections);
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
    printf("=========\n");
    printf("Heap Base: %p\n", heap->base);
    printf("Heap Top: %p\n", heap->top);
    printf("Heap Limit: %p\n", heap->limit);
    heap->collector();
    printf("Heap Base: %p\n", heap->base);
    printf("Heap Top: %p\n", heap->top);
    printf("Heap Limit: %p\n", heap->limit);

    if (heap->top + sizeof(_block_header) + nbytes > heap->limit) {
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
