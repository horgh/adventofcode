#include <assert.h>
#include "heap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void
sift_up(struct Heap * const h);

static void
sift_down(struct Heap * const h);

struct Heap *
heap_create(size_t const initial_sz)
{
	struct Heap * const h = calloc(1, sizeof(struct Heap));
	assert(h != NULL);

	h->elements = calloc(initial_sz, sizeof(struct HeapElement));
	assert(h->elements != NULL);

	h->sz = initial_sz;

	return h;
}

void
heap_free(struct Heap * const h)
{
	free(h->elements);
	free(h);
}

void
heap_insert(struct Heap * const h,
		int64_t const priority,
		void * const data)
{
	if (h->n_elements == h->sz) {
		h->sz = h->sz * 2; // TODO(horgh): Consider overflow
		h->elements  = realloc(h->elements, h->sz);
		assert(h->elements != NULL);
		// TODO(horgh): Zero new memory
	}

	size_t const idx = h->n_elements;
	h->elements[idx].priority = priority;
	h->elements[idx].data = data;
	h->n_elements++;

	sift_up(h);
}

static void
sift_up(struct Heap * const h)
{
	size_t const start = 0;
	size_t child = h->n_elements - 1;
	while (child > start) {
		size_t const parent = (child-1)/2;
		if (h->elements[parent].priority < h->elements[child].priority) {
			return;
		}
		struct HeapElement const e = h->elements[parent];
		h->elements[parent] = h->elements[child];
		h->elements[child] = e;
		child = parent;
	}
}

void *
heap_extract(struct Heap * const h)
{
	if (h->n_elements == 0) {
		return NULL;
	}
	struct HeapElement const he = h->elements[0];
	h->elements[0] = h->elements[h->n_elements - 1];
	h->n_elements--;
	sift_down(h);
	return he.data;
}

static void
sift_down(struct Heap * const h)
{
	size_t root = 0;
	if (h->n_elements == 0) {
		return;
	}
	size_t const end = h->n_elements - 1;
	while (1) {
		size_t const left = 2*root + 1;
		if (left > end) {
			return;
		}

		size_t swap = root;

		if (h->elements[swap].priority > h->elements[left].priority) {
			swap = left;
		}

		size_t const right = left+1;
		if (right <= end &&
				h->elements[swap].priority > h->elements[right].priority) {
			swap = right;
		}

		if (swap == root) {
			return;
		}

		struct HeapElement const e = h->elements[root];
		h->elements[root] = h->elements[swap];
		h->elements[swap] = e;
		root = swap;
	}
}

#ifdef TEST_HEAP

#include <assert.h>

int
main(void)
{
	// Test empty heap.

	struct Heap * h = heap_create(10);
	assert(h != NULL);
	void * data = heap_extract(h);
	assert(data == NULL);
	heap_free(h);

	// Test inserting one element and not extracting it.

	h = heap_create(10);
	assert(h != NULL);
	int mydata1 = 10;
	heap_insert(h, 1, &mydata1);
	heap_free(h);

	// Test inserting one element and extracting it.

	h = heap_create(10);
	assert(h != NULL);
	heap_insert(h, 1, &mydata1);

	data = heap_extract(h);
	assert(data == &mydata1);

	data = heap_extract(h);
	assert(data == NULL);

	heap_free(h);

	// Test inserting two elements with different priorities and
	// extracting them.

	h = heap_create(10);
	assert(h != NULL);
	int mydata2 = 20;
	heap_insert(h, 1, &mydata1);
	heap_insert(h, 2, &mydata2);

	data = heap_extract(h);
	assert(data == &mydata1);

	data = heap_extract(h);
	assert(data == &mydata2);

	data = heap_extract(h);
	assert(data == NULL);

	heap_free(h);

	// Test inserting two elements with the same priority.

	h = heap_create(10);
	assert(h != NULL);
	heap_insert(h, 1, &mydata1);
	heap_insert(h, 1, &mydata2);

	data = heap_extract(h);
	assert(data == &mydata2);

	data = heap_extract(h);
	assert(data == &mydata1);

	data = heap_extract(h);
	assert(data == NULL);

	heap_free(h);

	// Test inserting in different orders of priority.

	h = heap_create(10);
	assert(h != NULL);
	int mydata3 = 30;
	heap_insert(h, 3, &mydata3);
	heap_insert(h, 1, &mydata1);
	heap_insert(h, 2, &mydata2);

	data = heap_extract(h);
	assert(data == &mydata1);

	data = heap_extract(h);
	assert(data == &mydata2);

	data = heap_extract(h);
	assert(data == &mydata3);

	data = heap_extract(h);
	assert(data == NULL);

	heap_free(h);

	return 0;
}

#endif
