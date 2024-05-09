#include "globals.h"
#include "bistree.h"
#include <stdio.h>
#include <assert.h>

void print_mark_sweep_statistics() {
    printf("=========================\n");
    printf("Mark-Sweep Statistics\n");
    printf("Garbage collections so far: %d\n", garbageCollections);
    printf("Inserted Nodes Before This GC: %d\n", insertedNodes);
    printf("Total number of inserted nodes so far: %d\n", totalInsertedNodes);
    printf("Removed Nodes Before This GC: %d\n", removedNodes);
    printf("Total number of removed nodes so far: %d\n", totalRemovedNodes);
    printf("Marked Nodes: %d\n", markedNodes);
    printf("Swept Nodes: %d\n", sweptNodes);
    printf("Percentage of Heap Cleared: %f\n", ((float)sweptNodes / ((float)BLOCKS_IN_HEAP)) * 100.0);
    printf("=========================\n\n");

    assert(sweptNodes == removedNodes);
    assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}

void print_mark_compact_statistics() {
    printf("=========================\n");
    printf("Mark-Compact Statistics\n");
    printf("Garbage collections so far: %d\n", garbageCollections);
    printf("Inserted Nodes Before This GC: %d\n", insertedNodes);
    printf("Total number of inserted nodes so far: %d\n", totalInsertedNodes);
    printf("Removed Nodes Before This GC: %d\n", removedNodes);
    printf("Total number of removed nodes so far: %d\n", totalRemovedNodes);
    printf("Marked Nodes: %d\n", markedNodes);
    printf("Percentage of Heap Cleared: %f\n", (1 - ((float)((heap->top - heap->base) / BLOCK_SIZE) / ((float)BLOCKS_IN_HEAP))) * 100.0);
    printf("=========================\n");

    assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}

void print_copy_collection_statistics() {
    printf("=========================\n");
    printf("Copy and Collect Statistics\n");
    printf("Garbage collections so far: %d\n", garbageCollections);
    printf("Inserted Nodes Before This GC: %d\n", insertedNodes);
    printf("Total number of inserted nodes so far: %d\n", totalInsertedNodes);
    printf("Removed Nodes Before This GC: %d\n", removedNodes);
    printf("Total number of removed nodes so far: %d\n", totalRemovedNodes);
    printf("Percentage of Heap Cleared: %lf\n", (((float)removedNodes) / (heap->size / 2 / BLOCK_SIZE)) * 100.0f);
    printf("=========================\n");

    // assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}

void print_generational_statistics() {
    printf("=========================\n");
    printf("Generational Statistics\n");
    printf("Garbage collections so far: %d\n", garbageCollections);
    printf("Inserted Nodes Before This GC: %d\n", insertedNodes);
    printf("Total number of inserted nodes so far: %d\n", totalInsertedNodes);
    printf("Removed Nodes Before This GC: %d\n", removedNodes);
    printf("Total number of removed nodes so far: %d\n", totalRemovedNodes);
    printf("[WIP]Percentage of Heap Cleared: %lf\n", (((float)removedNodes) / (heap->size / 2 / BLOCK_SIZE)) * 100.0f);
    printf("=========================\n");

    // assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}