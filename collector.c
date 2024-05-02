/*
 * collector.c
 */

#include <stdio.h>
#include "bistree.h"
#include "list.h"
#include <stdlib.h>
#include "heap.h"
#include "globals.h"

int markRoots(List* roots);
int markTree(BiTreeNode* node);

int sweep();

void mark_sweep_gc(List* roots) {
    printf("Starting gc, before marking\n");
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reavoidble
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

    // printf("Resetting feeeb\n");
    // reset_freeb(heap);
    printf("Before sweep\n");
    int swept = sweep(heap);
    printf("Swept nodes: %d\n", swept);
    // printf("List size after sweep: %d\n", heap->freeb->size);

    printf("gcing()...\n");
    return;
}

void mark_compact_gc(List* roots) {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reavoidble
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
     * copy reavoidble to to_space
     */
    printf("gcing()...\n");
    return;
}

int markRoots(List* roots) {
    int sum = 0;
    // printf("roots: %d\n", roots->size);
    ListNode* cur_tree = roots->first;
    while (cur_tree != NULL) {
        BisTree* tree = (BisTree*)cur_tree->data;
        sum += markTree(tree->root);
        cur_tree = cur_tree->next;
    }

    return sum;
}

int markTree(BiTreeNode* node) {
    if (node == NULL)
        return 0;

    if (node->left != NULL && node->right != NULL) {
        if (node->left->data >= node->right->data) {
            printf("1Tree node is not ordered: %d, %d, %d\n", node->left->data, node->right->data, node->data);
            abort();
        }

        if (node->left->data >= node->data) {
            printf("2Tree node is not ordered: %d, %d\n", node->left->data, node->data);
            abort();
        }

        if (node->right->data <= node->data) {
            printf("3Tree node is not ordered: %d, %d\n", node->right->data, node->data);
            abort();
        }
    }
    // _block_header* header = (_block_header*)((void*)node - sizeof(_block_header));
    // printf("Marking node at: %p, header at: %p\n", (void*)node, (void*)header);
    _block_header* header = header_from_block(node);
    if (header->marked == 1) {
        printf("Tree node is already marked, somethings wrong I can feel it: %p\n", (void*)header);
        printf("Node data: %d\n", node->data);
        printf("Node left: %p\n", (void*)node->left);
        printf("Node right: %p\n", (void*)node->right);
        printf("%d\n", node->left == NULL);
        // abort();
    }

    header->marked = 1;
    return 1 + markTree(node->left) + markTree(node->right);
}

int sweep() {
    _block_header* header = (_block_header*)heap->base;
    _block_header* limit = (_block_header*)heap->top;
    int count = 0;
    while (header < limit) {
        if (header->marked == 0) {
            // printf("Adding to list\n");
            list_addlast(heap->freeb, (void*)header);
            count++;
        }

        header->marked = 0;
        header = next_header(header);
        // header = (_block_header*)((void*)header + sizeof(_block_header) + sizeof(BiTreeNode));
    }
    return count;
}