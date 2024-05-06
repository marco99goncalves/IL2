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
char* computeLocations();
void updateReferences();
void relocate();

void mark_sweep_gc() {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */
    markedNodes = markRoots(roots);

    sweptNodes = sweep();

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

    header->marked = 1;

    return 1 + markTree(node->left) + markTree(node->right);
}

int sweep() {
    _block_header* header = (_block_header*)heap->base;
    _block_header* top = (_block_header*)heap->top;

    int total = 0;
    while (header < top) {
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
    markedNodes = markRoots(roots);

    char* next_heap_top = computeLocations();

    updateReferences();

    relocate();

    heap->top = (char*)NEXT_HEADER((_block_header*)next_heap_top);

    /*
     * compact phase:
     * go through entire heap,
     * compute new addresses
     * copy objects to new addresses
     */
    printf("gcing()...\n");
    return;
}

char* computeLocations() {
    _block_header* scan = (_block_header*)heap->base;
    _block_header* end = (_block_header*)heap->top;
    _block_header* free = (_block_header*)heap->base;

    while (scan < end) {
        if (scan->marked == 1) {
            scan->next_free_block = (char*)free;
            free = NEXT_HEADER(free);
        }
        scan = NEXT_HEADER(scan);
    }
    return (char*)free;
}

void updateReferences() {
    ListNode* root = roots->first;
    _block_header* scan = (_block_header*)heap->base;
    _block_header* end = (_block_header*)heap->top;

    while (root != NULL) {
        BisTree* tree = (BisTree*)root->data;
        if (tree->root != NULL) {
            _block_header* tree_header = GET_HEADER_FROM_NODE(tree->root);
            tree->root = (BiTreeNode*)(tree_header->next_free_block + sizeof(_block_header));
        }

        root = root->next;
    }

    while (scan < end) {
        if (scan->marked == 1) {
            BiTreeNode* node = GET_NODE_FROM_HEADER(scan);
            // BiTreeNode* node = (BiTreeNode*)((char*)scan + sizeof(_block_header));

            if (node->left != NULL) {
                _block_header* left_header = GET_HEADER_FROM_NODE(node->left);
                node->left = (BiTreeNode*)(left_header->next_free_block + sizeof(_block_header));
            }

            if (node->right != NULL) {
                _block_header* right_header = GET_HEADER_FROM_NODE(node->right);
                node->right = (BiTreeNode*)(right_header->next_free_block + sizeof(_block_header));
            }
        }
        scan = NEXT_HEADER(scan);
    }
}

void relocate() {
    _block_header* scan = (_block_header*)heap->base;
    _block_header* end = (_block_header*)heap->limit;

    while (scan < end) {
        if (scan->marked == 1) {
            _block_header* dest = (_block_header*)scan->next_free_block;

            memmove(dest, scan, sizeof(_block_header) + scan->size);

            heap->top = (char*)dest;
            dest->marked = 0;
            dest->next_free_block = NULL;
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