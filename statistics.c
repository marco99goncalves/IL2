#include "globals.h"
#include "bistree.h"
#include <stdio.h>

void print_mark_sweep_statistics() {
    printf("=========================\n");
    printf("Mark-Sweep Statistics\n");
    printf("Garbage collections so far: %d\n", garbageCollections);
    printf("Inserted Nodes: %d\n", insertedNodes);
    printf("Removed Nodes: %d\n", removedNodes);
    printf("Marked Nodes: %d\n", markedNodes);
    printf("Swept Nodes: %d\n", sweptNodes);
    printf("Percentage of Heap Cleared: %f\%\n", ((float)sweptNodes / ((float)BLOCKS_IN_HEAP)) * 100.0);
    printf("=========================\n");

    insertedNodes = 0;
    removedNodes = 0;
}