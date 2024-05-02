/*
 * collector.c
 */

#include <stdio.h>
#include "list.h"
#include "bistree.h"
#include "globals.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int markRoots();
int markTree(BiTreeNode* node);
int sweep();
void computeLocations(_block_header* start, _block_header* end, _block_header* toRegion);
void updateReferences(_block_header* start, _block_header* end);
void relocate(_block_header* start, _block_header* end);

void mark_sweep_gc() {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */
    printf("inserted nodes: %d\n", insertedNodes);
    printf("removed nodes: %d\n", removedNodes);

    int markedNodes = markRoots(roots);
    printf("Marked Nodes: %d\n", markedNodes);

    int sweptNodes = sweep();
    assert(sweptNodes == removedNodes);
    printf("Swept Nodes: %d\n", sweptNodes);

    insertedNodes = 0;
    removedNodes = 0;

    /*
     * sweep phase:
     * go through entire heap,
     * add unmarked to free list
     */
    printf("gcing()...\n");
    return;
}

int markRoots() {
    int markedNodes = 0;
    ListNode* node = roots->first;
    while (node != NULL) {
        BisTree* tree = (BisTree*)node->data;
        markedNodes += markTree(tree->root);
        node = node->next;
    }
    return markedNodes;
}

int markTree(BiTreeNode* node) {
    if (node == NULL)
        return 0;

    _block_header* header = GET_HEADER_FROM_NODE(node);

    // TODO: REMOVE
    if (header->marked == 1) {
        printf("SANITY CHECK\n");
        exit(1);
    }

    header->marked = 1;

    return 1 + markTree(node->left) + markTree(node->right);
}

int sweep() {
    _block_header* header = (_block_header*)heap->base;
    _block_header* top = (_block_header*)heap->limit;

    int total = 0;
    while (BLOCK_LIMIT(header) < top) {
        if (header->marked == 0) {
            total++;
            add(header);
        }

        header->marked = 0;
        header = NEXT_HEADER(header);
    }
    return total;
}

void mark_compact_gc() {
    printf("inserted nodes: %d\n", insertedNodes);
    printf("removed nodes: %d\n", removedNodes);

    int markedNodes = markRoots(roots);
    printf("Marked Nodes: %d\n", markedNodes);

    computeLocations((_block_header*)heap->base, (_block_header*)heap->limit, (_block_header*)heap->base);
    printf("Computed Locations\n");
    updateReferences((_block_header*)heap->base, (_block_header*)heap->limit);
    printf("Updated References\n");
    relocate((_block_header*)heap->base, (_block_header*)heap->limit);
    printf("Relocated\n");
    heap->top = heap->top + sizeof(_block_header) + sizeof(BiTreeNode);

    /*
     * compact phase:
     * go through entire heap,
     * compute new addresses
     * copy objects to new addresses
     */
    printf("gcing()...\n");
    return;
}

void computeLocations(_block_header* start, _block_header* end, _block_header* toRegion) {
    _block_header* scan = start;
    _block_header* free = toRegion;
    while (BLOCK_LIMIT(scan) < end) {
        if (scan->marked == 1) {
            scan->next_free_block = free;
            free = NEXT_HEADER(free);
        }
        scan = NEXT_HEADER(scan);
    }
}

void updateReferences(_block_header* start, _block_header* end) {
    ListNode* root = roots->first;
    while (root != NULL) {
        BisTree* tree = (BisTree*)root->data;
        if (tree->root != NULL) {
            _block_header* tree_header = GET_HEADER_FROM_NODE((tree->root));
            tree->root = tree_header->next_free_block;
        }

        root = root->next;
    }

    _block_header* scan = start;
    while (BLOCK_LIMIT(scan) < end) {
        if (scan->marked == 1) {
            BiTreeNode* node = GET_NODE_FROM_HEADER(scan);
            if (node->left != NULL) {
                _block_header* l = GET_HEADER_FROM_NODE((node->left));
                node->left = l->next_free_block;
            }

            if (node->right != NULL) {
                _block_header* r = GET_HEADER_FROM_NODE((node->right));
                node->right = r->next_free_block;
            }
        }
        scan = NEXT_HEADER(scan);
    }
}

void relocate(_block_header* start, _block_header* end) {
    _block_header* scan = start;
    while (BLOCK_LIMIT(scan) < end) {
        if (scan->marked == 1) {
            _block_header* dest = scan->next_free_block;
            memcpy(dest, scan, sizeof(_block_header) + sizeof(BiTreeNode));
            // printf("Relocating %p to %p\n", scan, dest);
            heap->top = (char*)dest;
            dest->marked = 0;
        }
        scan = NEXT_HEADER(scan);
    }
}

void copy_collection_gc() {
    /*
     * go throught all roots,
     * traverse trees in from_space,
     * copy reachable to to_space
     */
    printf("gcing()...\n");
    return;
}