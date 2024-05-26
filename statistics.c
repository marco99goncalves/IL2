#include "globals.h"
#include "bistree.h"
#include <stdio.h>
#include <assert.h>

void print_mark_sweep_statistics() {
    printf("%s============ Mark And Sweep =============%s\n", CYAN, NORMAL);
    printf("Garbage collections so far: %s%d%s\n", MAGENT, garbageCollections, NORMAL);
    printf("Inserted Nodes Before This GC: %s%d%s\n", MAGENT, insertedNodes, NORMAL);
    printf("Total number of inserted nodes so far: %s%d%s\n", MAGENT, totalInsertedNodes, NORMAL);
    printf("Removed Nodes Before This GC: %s%d%s\n", MAGENT, removedNodes, NORMAL);
    printf("Total number of removed nodes so far: %s%d%s\n", MAGENT, totalRemovedNodes, NORMAL);
    printf("Marked Nodes: %s%d%s\n", MAGENT, markedNodes, NORMAL);
    printf("Swept Nodes: %s%d%s\n", MAGENT, sweptNodes, NORMAL);
    printf("Space freed in MB: %s%0.2lf%s\n", MAGENT, ((float)sweptNodes * BLOCK_SIZE) / (1024 * 1024), NORMAL);
    printf("Percentage of Heap Free: %s%0.2lf%%%s\n", MAGENT, ((float)sweptNodes / ((float)BLOCKS_IN_HEAP)) * 100.0, NORMAL);
    printf("%s===========================================%s\n\n", CYAN, NORMAL);

    assert(sweptNodes == removedNodes);
    assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}

void print_mark_compact_statistics() {
    printf("%s============ Mark And Compact =============%s\n", CYAN, NORMAL);
    printf("Garbage collections so far: %s%d%s\n", MAGENT, garbageCollections, NORMAL);
    printf("Inserted Nodes Before This GC: %s%d%s\n", MAGENT, insertedNodes, NORMAL);
    printf("Total number of inserted nodes so far: %s%d%s\n", MAGENT, totalInsertedNodes, NORMAL);
    printf("Removed Nodes Before This GC: %s%d%s\n", MAGENT, removedNodes, NORMAL);
    printf("Total number of removed nodes so far: %s%d%s\n", MAGENT, totalRemovedNodes, NORMAL);
    printf("Marked Nodes: %s%d%s\n", MAGENT, markedNodes, NORMAL);
    printf("Space freed in MB: %s%0.2lf%s\n", MAGENT, ((float)removedNodes * BLOCK_SIZE) / (1024 * 1024), NORMAL);
    printf("Percentage of Heap Cleared: %s%f%%%s\n", MAGENT, (1 - ((float)((heap->top - heap->base) / BLOCK_SIZE) / ((float)BLOCKS_IN_HEAP))) * 100.0, NORMAL);
    printf("%s===========================================%s\n\n", CYAN, NORMAL);

    assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}

void print_copy_collection_statistics() {
    printf("%s============ Copy and Collect =============%s\n", CYAN, NORMAL);
    printf("Garbage collections so far: %s%d%s\n", MAGENT, garbageCollections, NORMAL);
    printf("Inserted Nodes Before This GC: %s%d%s\n", MAGENT, insertedNodes, NORMAL);
    printf("Total number of inserted nodes so far: %s%d%s\n", MAGENT, totalInsertedNodes, NORMAL);
    printf("Removed Nodes Before This GC: %s%d%s\n", MAGENT, removedNodes, NORMAL);
    printf("Total number of removed nodes so far: %s%d%s\n", MAGENT, totalRemovedNodes, NORMAL);
    printf("Space freed in MB: %s%0.2lf%s\n", MAGENT, ((float)removedNodes * BLOCK_SIZE) / (1024 * 1024), NORMAL);
    printf("Percentage of Heap Cleared: %s%lf%%%s\n", MAGENT, (((float)removedNodes) / (heap->size / 2 / BLOCK_SIZE)) * 100.0f, NORMAL);
    printf("%s===========================================%s\n\n", CYAN, NORMAL);

    // assert((totalInsertedNodes - totalRemovedNodes) == markedNodes);

    insertedNodes = 0;
    removedNodes = 0;
}