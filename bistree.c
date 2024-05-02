/*
 * bistree.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "bistree.h"
#include "heap.h"

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
            printf("well shit\n");
            printf("Node: %p\n", node);
            exit(1);
        }

        // printf("Node: %p\n", node);
        node->data = data;
        // printf("Data: %d\n", node->data);
        node->left = NULL;
        node->right = NULL;
        // printf("Everyhing okay\n");
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
    printf("root: %p\n", tree);
    bitreenode_inorder(tree->root);
    printf("\n");
}