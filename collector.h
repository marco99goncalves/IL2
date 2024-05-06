/*
 * collector.h
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

void mark_sweep_gc();

void mark_compact_gc();

void copy_collection_gc();

void generational_gc();

#endif
