/*
 * collector.c
 */

#include <stdio.h>

#include "list.h"

void mark_sweep_gc(List* roots) {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
     */
    markRoots(roots);

    /*
     * sweep phase:
     * go through entire heap,
     * add unmarked to free list
     */
    printf("gcing()...\n");
    return;
}

void mark_compact_gc(List* roots) {
    /*
     * mark phase:
     * go throught all roots,
     * traverse trees,
     * mark reachable
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
     * copy reachable to to_space
     */
    printf("gcing()...\n");
    return;
}

void markRoots(List* roots) {
}