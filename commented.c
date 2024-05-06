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
void computeLocations();
void updateReferences();
void relocate();
void checkNodes(List* roots);
void checkTree(BiTreeNode* node);

void mark_sweep_gc() {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */
    // printf("inserted nodes: %d\n", insertedNodes);
    // printf("removed nodes: %d\n", removedNodes);

    markedNodes = markRoots(roots);
    // printf("Marked Nodes: %d\n", markedNodes);

    sweptNodes = sweep();
    assert(sweptNodes == removedNodes);
    // printf("Swept Nodes: %d\n", sweptNodes);

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
        printf("SUS\n");
        printf("SANITY CHECK\n");
        printf("already marked\n");

        printf("header: %p\n", header);
        printf("node: %p\n", node);
        printf("heap->base: %p\n", heap->base);
        printf("heap->top: %p\n", heap->top);
        printf("heap->limit: %p\n", heap->limit);
        printf("left: %p\n", node->left);
        printf("right: %p\n", node->right);
        printf("distance from heap->base in blocks: %ld\n", ((char*)header - heap->base) / BLOCK_SIZE);

        if ((((char*)header - heap->base) % BLOCK_SIZE != 0) || ((char*)node - heap->base) % BLOCK_SIZE != 16) {
            printf("SUS\n");
            printf("header: %p\n", header);
            printf("heap->base: %p\n", heap->base);
            printf("heap->limit: %p\n", heap->limit);
            exit(1);
        }

        exit(1);
    }

    header->marked = 1;

    return 1 + markTree(node->left) + markTree(node->right);
}

int sweep() {
    _block_header* header = (_block_header*)heap->base;
    _block_header* top = (_block_header*)heap->limit;

    int total = 0;
    while (BLOCK_LIMIT(header) < (char*)top) {
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
    printf("Total nodes in heap in theory: %d\n", totalInsertedNodes - totalRemovedNodes);

    int markedNodes = markRoots(roots);
    printf("Marked Nodes: %d\n", markedNodes);

    computeLocations();
    printf("Computed Locations\n");

    printf("Checking if all free blocks are in the heap\n");
    _block_header* scan = (_block_header*)heap->base;
    while (BLOCK_LIMIT(scan) < heap->limit) {
        if (scan->marked == 1 && (scan->next_free_block < (char*)heap->base || BLOCK_LIMIT(scan->next_free_block) > heap->limit || scan->next_free_block == NULL)) {
            printf("SUS\n");
            printf("scan->next_free_block: %p\n", scan->next_free_block);
            printf("heap->base: %p\n", heap->base);
            printf("heap->limit: %p\n", heap->limit);
            abort();
        }
        scan = NEXT_HEADER(scan);
    }

    updateReferences();
    printf("Updated References\n");

    relocate();
    printf("Relocated\n");
    heap->top = (char*)NEXT_HEADER((_block_header*)heap->top);

    printf("Checking if all blocks have been marked wiht 0\n");
    scan = (_block_header*)heap->base;
    while (BLOCK_LIMIT(scan) < heap->limit) {
        if (scan->marked == 1) {
            printf("SUS checked marked\n");
            printf("scan->next_free_block: %p\n", scan->next_free_block);
            printf("heap->base: %p\n", heap->base);
            printf("heap->limit: %p\n", heap->limit);
            exit(1);
        }
        scan = NEXT_HEADER(scan);
    }

    printf("Checking nodes\n");
    checkNodes(roots);

    printf("Checking if all blocks have been marked wiht 0 again\n");
    scan = (_block_header*)heap->base;
    while (BLOCK_LIMIT(scan) < heap->limit) {
        if (scan->marked == 1) {
            printf("SUS\n");
            printf("scan->next_free_block: %p\n", scan->next_free_block);
            printf("heap->base: %p\n", heap->base);
            printf("heap->limit: %p\n", heap->limit);
            exit(1);
        }
        scan = NEXT_HEADER(scan);
    }

    // printf("ri\n");
    // exit(1);

    /*
     * compact phase:
     * go through entire heap,
     * compute new addresses
     * copy objects to new addresses
     */
    printf("gcing()...\n");
    return;
}

void checkNodes(List* roots) {
    ListNode* node = roots->first;
    int t = 0;
    while (node != NULL) {
        BisTree* tree = (BisTree*)node->data;
        // printf("Checking tree %d\n", t++);
        checkTree(tree->root);
        node = node->next;
    }
}

void checkTree(BiTreeNode* node) {
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

    checkTree(node->left);
    checkTree(node->right);
}

void computeLocations() {
    _block_header* scan = (_block_header*)heap->base;
    _block_header* end = (_block_header*)heap->limit;
    _block_header* free = (_block_header*)heap->base;
    int sum = 0;
    while (BLOCK_LIMIT(scan) < (char*)end) {
        if (scan->marked == 1) {
            scan->next_free_block = free;
            free = NEXT_HEADER(free);
        }
        scan = NEXT_HEADER(scan);
        sum++;
    }

    printf("sum: %d\n", sum);
}

void updateReferences() {
    ListNode* root = roots->first;
    _block_header* scan = (_block_header*)heap->base;
    _block_header* limit = (_block_header*)heap->limit;

    while (root != NULL) {
        BisTree* tree = (BisTree*)root->data;
        if (tree->root != NULL) {
            _block_header* tree_header = GET_HEADER_FROM_NODE(tree->root);
            tree->root = (BiTreeNode*)(tree_header->next_free_block + sizeof(_block_header));
        }

        root = root->next;
    }

    scan = (_block_header*)heap->base;
    limit = (_block_header*)heap->limit;
    while (BLOCK_LIMIT(scan) < limit) {
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

void relocate() {
    _block_header* scan = (_block_header*)heap->base;
    _block_header* limit = (_block_header*)heap->limit;
    heap->top = (char*)heap->base;
    int sum = 0;
    while (BLOCK_LIMIT(scan) < (char*)limit) {
        if (scan->marked == 1) {
            _block_header* dest = scan->next_free_block;

            memmove((char*)dest + sizeof(_block_header), (char*)scan + sizeof(_block_header), sizeof(BiTreeNode));

            // memcpy((char*)dest, (char*)scan, sizeof(_block_header) + sizeof(BiTreeNode));
            // printf("Relocating %p to %p\n", scan, dest);
            heap->top = (char*)dest;
            dest->marked = 0;
            dest->next_free_block = NULL;
        }
        scan->marked = 0;
        scan->next_free_block = NULL;
        scan = NEXT_HEADER(scan);
    }
    printf("relocated: %d\n", sum);
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