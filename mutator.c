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

#define MAX_NODES 100
#define MIN_NODES 5
#define MAX_KEY_VALUE 100

#define HEAP_SIZE (1024 * 1024) /* 1 MByte */

Heap* heap;
List* roots;
void* free_blocks;
int insertedNodes;
int removedNodes;
int garbageCollections;
int markedNodes;
int sweptNodes;
int totalInsertedNodes;
int totalRemovedNodes;

int GC_ALGORITHM = COPY_COLLECT;

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

    if (argc == 3)
        GC_ALGORITHM = atoi(argv[2]);

    float threshold = atof(argv[1]); /* a value in the interval (0,1) */

    heap = (Heap*)malloc(sizeof(Heap));
    roots = (List*)malloc(sizeof(List));
    insertedNodes = 0;
    removedNodes = 0;
    garbageCollections = 0;
    markedNodes = 0;
    sweptNodes = 0;
    totalInsertedNodes = 0;
    totalRemovedNodes = 0;

    heap_init(heap, HEAP_SIZE);
    list_init(roots);

    srandom(getpid());
    mutate = true;

    while (mutate) {
        float toss = (float)random() / RAND_MAX;
        if (toss > threshold) {  // add nodes
            /* build new bistree */
            BisTree* t = (BisTree*)malloc(sizeof(BisTree));
            bistree_init(t);
            /* preserve root */
            list_addlast(roots, t);
            /* prepare to insert up to 100 nodes, a minimum of 5 */
            int number_nodes = MIN_NODES + random() % (MAX_NODES - MIN_NODES);
            for (int i = 0; i < number_nodes; i++) {
                /* populate tree with keys between 0-100 */
                int key = random() % MAX_KEY_VALUE;
                // printf("Inserting %d\n", key);
                if (bistree_insert(t, key)) {
                    insertedNodes++;
                    totalInsertedNodes++;
                }
            }

            // fprintf(stdout, "inserted %d nodes\n", insertedNodes);
            // fprintf(stdout, "tree size is %d\n", bistree_size(t));
            // fprintf(stdout, "(inorder traversal)\n");
            // bistree_inorder(t);
        } else {  // remove nodes
            /* skip if there are no roots to manipulate */
            if (list_isempty(roots))
                continue;
            /* otherwise, choose random root to operate on */
            int index = random() % list_size(roots);
            BisTree* chosen = list_get(roots, index);
            int number_nodes = bistree_size(chosen);
            int number_tries = random() % number_nodes;
            for (int i = 0; i < number_tries; i++) {
                /* remove key from tree if key exists in it */
                /* this is checked in bistree_remove */
                if (bistree_remove(chosen, random() % MAX_KEY_VALUE)) {
                    removedNodes++;
                    totalRemovedNodes++;
                }
            }
            // fprintf(stdout, "removed %d nodes\n", removedNodes);
            // fprintf(stdout, "tree size is %d\n", bistree_size(chosen));
            // fprintf(stdout, "(inorder traversal)\n");
            // bistree_inorder(chosen);
        }
    }
    /* caught ^C ! */
    printf("quiting...\n");
    heap_destroy(heap);
    return 0;
}
