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

// Helper functions

// General
int markRoots();
int markTree(BiTreeNode* node);

// Mark and Sweep
int sweep();

// Mark and Compact
char* computeLocations();
void updateReferences();
void relocate();

// Copy and Collect
void flip();
void swap(char** a, char** b);
char* forward(BiTreeNode* node);
char* copy(BiTreeNode* node);
void scan(char* ref);

void initialize_list();
bool is_list_empty();
char* remove_from_list();

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
    // printf("\n\n\n");
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
    // printf("\n\n\n");
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

void create_semi_spaces() {
    heap->toSpace = heap->base;
    heap->extent = (heap->limit - heap->base) / 2;
    heap->fromSpace = heap->base + heap->extent;
    heap->limit = heap->fromSpace;
    heap->top = heap->toSpace;
}

void copy_collection_gc() {
    /*
     * go throught all roots,
     * traverse trees in from_space,
     * copy reachable to to_space
     */

    flip();

    initialize_list();

    ListNode* root = roots->first;
    int sum = 0;
    while (root != NULL) {
        BisTree* tree = (BisTree*)root->data;
        if (tree->root != NULL) {
            tree->root = GET_NODE_FROM_HEADER((_block_header*)forward(tree->root));
        } else {
            sum++;
        }

        root = root->next;
    }

    while (!is_list_empty()) {
        _block_header* ref = (_block_header*)remove_from_list();

        BiTreeNode* node = GET_NODE_FROM_HEADER(ref);
        if (node->left != NULL)
            node->left = GET_NODE_FROM_HEADER((_block_header*)forward(node->left));

        if (node->right != NULL)
            node->right = GET_NODE_FROM_HEADER((_block_header*)forward(node->right));
    }

    return;
}

char* forward(BiTreeNode* node) {
    char* toRef = GET_HEADER_FROM_NODE(node)->next_free_block;
    if (toRef == NULL)
        toRef = copy(node);

    return toRef;
}

char* copy(BiTreeNode* node) {
    char* toRef = heap->top;
    _block_header* nodeHeader = GET_HEADER_FROM_NODE(node);
    heap->top = (char*)NEXT_HEADER(heap->top);  // TODO: Check why NEXT_HEADER DOESNT WORK

    memmove(toRef, nodeHeader, sizeof(_block_header) + nodeHeader->size);
    // nodeHeader->next_free_block = toRef;
    return toRef;
}

void flip() {
    swap(&heap->fromSpace, &heap->toSpace);
    heap->limit = heap->toSpace + heap->extent;
    heap->top = heap->toSpace;
}

void swap(char** a, char** b) {
    char* temp = *a;
    *a = *b;
    *b = temp;
}

void initialize_list() {
    heap->free = heap->top;
}

bool is_list_empty() {
    return heap->free == heap->top;
}

char* remove_from_list() {
    char* ref = heap->free;
    heap->free = (char*)NEXT_HEADER(heap->free);
    return ref;
}