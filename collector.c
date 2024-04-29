/*
 * collector.c
 */

#include <stdio.h>
#include "bistree.h"
#include "list.h"
#include <stdlib.h>
#include "heap.h"

int markRoots(List* roots);
int markTree(BiTreeNode* node);

int sweep(Heap* heap);

void mark_sweep_gc(List* roots, void* heapv) {
    Heap* heap = (Heap*)heapv;  // TODO: gross
    printf("Starting gc, before marking\n");
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */
    int marked = markRoots(roots);
    printf("Number of roots oficial: %d\n", roots->size);
    printf("Number of roots marked: %d\n", marked);

    int sum = 0;
    for (int i = 0; i < roots->size; i++) {
        BisTree* tree = (BisTree*)list_get(roots, i);
        sum += tree->size;
    }
    printf("Total nodes: %d\n", sum);
    printf("Expected cleanup: %d\n", sum - marked);

    /*
     * sweep phase:
     * go through entire heap,
     * add unmarked to free list
     */

    reset_freeb(heap);
    printf("Before sweep\n");
    int swept = sweep(heap);
    printf("Swept nodes: %d\n", swept);

    printf("gcing()...\n");
    return;
}

void mark_compact_gc(List* roots) {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */

    /*
     * compact phase:
     * go through entire heap,
     * compute new addresses
     * copy objects to new addresses
     */
    printf("gcing()...\n");
    return;
}

void copy_collection_gc(List* roots) {
    /*
     * go throught all roots,
     * traverse trees in from_space,
     * copy reachable to to_space
     */
    printf("gcing()...\n");
    return;
}

int markRoots(List* roots) {
    int sum = 0;
    // printf("roots: %d\n", roots->size);
    for (int i = 0; i < roots->size; i++) {
        BisTree* tree = (BisTree*)list_get(roots, i);
        sum += markTree(tree->root);
        // printf("ola\n");
    }
    // printf("OLA\n");
    return sum;
}

int markTree(BiTreeNode* node) {
    if (node == NULL)
        return 0;
    _block_header* header = &node - sizeof(_block_header);
    header->marked = 1;
    header->size = sizeof(BiTreeNode);
    return 1 + markTree(node->left) + markTree(node->right);
}

int sweep(Heap* heap) {
    _block_header* header = heap->base;
    int count = 0;
    while (header < heap->limit) {
        if (header->marked == 0) {
            list_addlast(heap->freeb, header);
            count++;
        }
        header += sizeof(_block_header) + sizeof(ListNode);
    }
    return count;
}