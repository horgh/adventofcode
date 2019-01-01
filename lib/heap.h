#ifndef ADVENT_HEAP_H
#define ADVENT_HEAP_H

#include <stddef.h>
#include <stdint.h>

// This is a minimum priority heap.

struct HeapElement {
	int64_t priority;
	size_t index;
	void * data;
};

struct Heap {
	struct HeapElement ** elements;
	size_t n_elements;
	size_t sz;
};

struct Heap * heap_create(size_t const initial_size);

void heap_free(struct Heap * const h);

struct HeapElement * heap_insert(
		struct Heap * const h, int64_t const priority, void * const data);

void * heap_extract(struct Heap * const h);

void heap_decrease_priority(struct Heap * const h,
		struct HeapElement * const he,
		int64_t const priority);

#endif
