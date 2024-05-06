/*
 * bistree.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "bistree.h"
#include "heap.h"
#include "globals.h"

void checkNodes3(List* roots);
void checkTree3(BiTreeNode* node);

void checkNodes3(List* roots) {
    ListNode* node = roots->first;
    int t = 0;
    while (node != NULL) {
        BisTree* tree = (BisTree*)node->data;
        // printf("Checking tree %d\n", t++);
        checkTree3(tree->root);
        node = node->next;
    }
}

void checkTree3(BiTreeNode* node) {
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

    checkTree3(node->left);
    checkTree3(node->right);
}

void bistree_init(BisTree* tree) {
    tree->root = NULL;
    tree->size = 0;
    return;
}

bool bitreenode_lookup(BiTreeNode* node, int data) {
    // printf("Looking for %d\n", data);
    // printf("Node: %p\n", node);
    if (node == NULL)
        return false;
    if (data < node->data)
        return bitreenode_lookup(node->left, data);
    if (data > node->data)
        return bitreenode_lookup(node->right, data);
    return true;
}

bool bistree_lookup(BisTree* tree, int data) {
    return bitreenode_lookup(tree->root, data);
}

BiTreeNode* bitreenode_insert(BiTreeNode* node, int data) {
    if (node == NULL) {
        // printf("Inserting node with value: %d\n", data);
        BiTreeNode* node = (BiTreeNode*)my_malloc(sizeof(BiTreeNode));
        if (node == NULL) {
            printf("Node: %p\n", node);
            exit(1);
        }

        if ((((((char*)node) - sizeof(_block_header)) - (heap->base)) % (BLOCK_SIZE)) != 0) {
            _block_header* header = GET_HEADER_FROM_NODE(node);
            if (header->marked == 1)
                abort();
            printf("SUS2\n");
            printf("node: %p\n", (char*)node - sizeof(_block_header));
            printf("%ld\n", ((((char*)node) - sizeof(_block_header)) - (heap->base)) % (BLOCK_SIZE));
            exit(1);
        }

        // printf("Node: %p\n", node);
        node->data = data;
        // printf("Data: %d\n", node->data);
        node->left = NULL;
        node->right = NULL;
        // printf("Everyhing okay\n");

        // printf("What am i doing...\n");
        // _block_header* scan = (_block_header*)heap->base;
        // while (BLOCK_LIMIT(scan) < heap->limit) {
        //     if (scan->marked == 1) {
        //         printf("SUS\n");
        //         printf("scan->next_free_block: %p\n", scan->next_free_block);
        //         printf("heap->base: %p\n", heap->base);
        //         printf("heap->limit: %p\n", heap->limit);
        //         exit(1);
        //     }
        //     scan = NEXT_HEADER(scan);
        // }
        // printf("No headers marked with 1\n");

        // printf("Checking nodes again\n");
        // checkNodes3(roots);
        // printf("Nodes checked\n");
        // printf("Inserted node at: %p\n", node);
        return node;
    } else if (data < node->data)
        node->left = bitreenode_insert(node->left, data);
    else
        node->right = bitreenode_insert(node->right, data);
    return node;
}

bool bistree_insert(BisTree* tree, int data) {
    if (bistree_lookup(tree, data))
        return false;
    tree->root = bitreenode_insert(tree->root, data);
    tree->size = tree->size + 1;

    return true;
}

BiTreeNode* bitreenode_remove(BiTreeNode* node, int data) {
    if (data < node->data)
        node->left = bitreenode_remove(node->left, data);
    else if (data > node->data)
        node->right = bitreenode_remove(node->right, data);
    else if (node->left == NULL)
        node = node->right;
    else if (node->right == NULL)
        node = node->left;
    else {
        BiTreeNode* lnode = node->left;
        while (lnode->right != NULL)
            lnode = lnode->right;
        node->data = lnode->data;
        node->left = bitreenode_remove(node->left, lnode->data);
        //   free(lnode);
    }
    return node;
}

bool bistree_remove(BisTree* tree, int data) {
    if (!bistree_lookup(tree, data))
        return false;
    tree->root = bitreenode_remove(tree->root, data);
    tree->size = tree->size - 1;
    return true;
}

void bitreenode_inorder(BiTreeNode* node) {
    if (node == NULL)
        return;
    bitreenode_inorder(node->left);
    printf(" %d ", node->data);
    bitreenode_inorder(node->right);
}

void bistree_inorder(BisTree* tree) {
    printf("base: %p\n", heap->base);
    printf("root: %p\n", tree->root);
    bitreenode_inorder(tree->root);
    printf("\n");
}