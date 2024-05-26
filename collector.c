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
int sweep(Heap* heap);

// Mark and Compact
char* computeLocations(Heap* heap);
void updateReferences(Heap* heap);
void relocate(Heap* heap);

// Copy and Collect
void create_semi_spaces(Heap* heap);
void flip(Heap* heap);
void swap(char** a, char** b);
char* forward(BiTreeNode* node);
char* copy(BiTreeNode* node, Heap* heap);

void initialize_list(Heap* heap);
bool is_list_empty(Heap* heap);
char* remove_from_list(Heap* heap);

void mark_sweep_gc(Heap* heap) {
    markedNodes = markRoots(roots);

    sweptNodes = sweep(heap);
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

int sweep(Heap* heap) {
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

void mark_compact_gc(Heap* heap) {
    markedNodes = markRoots(roots);

    char* next_heap_top = computeLocations(heap);

    updateReferences(heap);

    relocate(heap);

    heap->top = (char*)NEXT_HEADER((_block_header*)next_heap_top);
    return;
}

char* computeLocations(Heap* heap) {
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

void updateReferences(Heap* heap) {
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

void relocate(Heap* heap) {
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

void create_semi_spaces(Heap* heap) {
    heap->toSpace = heap->base;
    heap->extent = (heap->limit - heap->base) / 2;
    heap->fromSpace = heap->base + heap->extent;
    heap->limit = heap->fromSpace;
    heap->top = heap->toSpace;
}

void copy_collection_gc(Heap* heap) {
    flip(heap);

    initialize_list(heap);

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

    while (!is_list_empty(heap)) {
        _block_header* ref = (_block_header*)remove_from_list(heap);

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
        toRef = copy(node, heap);

    return toRef;
}

char* copy(BiTreeNode* node, Heap* heap) {
    char* toRef = heap->top;
    _block_header* nodeHeader = GET_HEADER_FROM_NODE(node);
    heap->top = (char*)NEXT_HEADER(heap->top);

    memmove(toRef, nodeHeader, sizeof(_block_header) + nodeHeader->size);
    return toRef;
}

void flip(Heap* heap) {
    swap(&heap->fromSpace, &heap->toSpace);
    heap->limit = heap->toSpace + heap->extent;
    heap->top = heap->toSpace;
}

void swap(char** a, char** b) {
    char* temp = *a;
    *a = *b;
    *b = temp;
}

void initialize_list(Heap* heap) {
    heap->free = heap->top;
}

bool is_list_empty(Heap* heap) {
    return heap->free == heap->top;
}

char* remove_from_list(Heap* heap) {
    char* ref = heap->free;
    heap->free = (char*)NEXT_HEADER(heap->free);
    return ref;
}