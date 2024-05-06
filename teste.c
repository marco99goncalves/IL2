/*
 * the mutator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "bool.h"
#include "heap.h"
#include "list.h"
#include "bistree.h"
#include "globals.h"
#include "collector.h"
#include <assert.h>

#define HEAP_SIZE (1024 * 1024) /* 1 MByte */

Heap* heap;
List* roots;
void* free_blocks;
int insertedNodes;
int removedNodes;
int garbageCollections;

static bool mutate;

static void sigint_handler() {
    mutate = false;
}

int main(int argc, char** argv) {
    /*
     * ^C to stop the program
     */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("signal: cannot install handler for SIGINT\n");
        return 1;
    }

    heap = (Heap*)malloc(sizeof(Heap));
    roots = (List*)malloc(sizeof(List));
    insertedNodes = 0;
    removedNodes = 0;
    garbageCollections = 0;

    heap_init(heap, HEAP_SIZE, mark_sweep_gc);
    list_init(roots);

    mutate = true;
    int targetNodes = ((float)(HEAP_SIZE / (sizeof(_block_header) + sizeof(BiTreeNode)))) * (3.0 / 4.0);
    BisTree* t = (BisTree*)malloc(sizeof(BisTree));
    bistree_init(t);

    printf("Target Nodes: %d\n", targetNodes);

    list_addfirst(roots, t);
    while (mutate) {
        printf("Adding %d nodes to tree\n", targetNodes);
        for (int i = 0; i < targetNodes; i++) {
            assert(bistree_insert(t, i));
            insertedNodes++;
        }

        printf("Removing %d nodes from tree\n");
        for (int i = 0; i < targetNodes; i++) {
            assert(bistree_remove(t, i));
            removedNodes++;
        }

        // printf("Adding %d nodes to tree\n", targetNodes);
        // for (int i = 0; i < targetNodes; i++) {
        //     assert(bistree_insert(t, i));
        // }
    }
    /* caught ^C ! */
    printf("quiting...\n");
    heap_destroy(heap);
    return 0;
}
