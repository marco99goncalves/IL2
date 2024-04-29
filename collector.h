/*
 * collector.h
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

void mark_sweep_gc(List* roots, void* heap);

void mark_compact_gc(List* roots, void* heap);

void copy_collection_gc(List* roots, void* heap);

void generational_gc(List* roots, void* heap, ...);

#endif
