#ifndef ADVENT_HEAP_H
#define ADVENT_HEAP_H

#include <stddef.h>
#include <stdint.h>

// This is a minimum priority heap.

struct HeapElement {
	int64_t priority;
	void * data;
};

struct Heap {
	struct HeapElement * elements;
	size_t n_elements;
	size_t sz;
};

struct Heap *
heap_create(size_t const initial_sz);

void
heap_free(struct Heap * const h);

void
heap_insert(struct Heap * const h,
		int64_t const priority,
		void * const data);

void *
heap_extract(struct Heap * const h);

#endif
