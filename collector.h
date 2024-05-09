/*
 * collector.h
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

void mark_sweep_gc(Heap* heap);

void mark_compact_gc(Heap* heap);

void copy_collection_gc(Heap* heap);
void create_semi_spaces(Heap* heap);

void generational_gc(Heap* heap);

#endif
