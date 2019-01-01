#include <assert.h>
#include "heap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void
sift_up(struct Heap * const h, size_t const end);

static void
sift_down(struct Heap * const h);

struct Heap *
heap_create(size_t const initial_size)
{
	struct Heap * const h = calloc(1, sizeof(struct Heap));
	assert(h != NULL);

	h->sz = initial_size;

	h->elements = calloc(h->sz, sizeof(struct HeapElement *));
	assert(h->elements != NULL);

	return h;
}

void
heap_free(struct Heap * const h)
{
	for (size_t i = 0; i < h->n_elements; i++) {
		free(h->elements[i]);
	}
	free(h->elements);
	free(h);
}

struct HeapElement *
heap_insert(struct Heap * const h,
		int64_t const priority,
		void * const data)
{
	if (h->n_elements == h->sz) {
		// TODO(horgh): Increase size. Note realloc() will move memory so all
		// pointers held by callers will be invalid.
		return NULL;
	}

	struct HeapElement * const he = calloc(1, sizeof(struct HeapElement));
	assert(he != NULL);

	size_t const index = h->n_elements;
	h->elements[index] = he;

	he->priority = priority;
	he->index = index;
	he->data = data;

	h->n_elements++;

	sift_up(h, index);

	return he;
}

static void
sift_up(struct Heap * const h, size_t const end)
{
	size_t const start = 0;
	size_t child = end;
	while (child > start) {
		size_t const parent = (child-1)/2;
		if (h->elements[parent]->priority < h->elements[child]->priority) {
			return;
		}

		struct HeapElement * const e = h->elements[parent];
		h->elements[parent] = h->elements[child];
		h->elements[child] = e;

		h->elements[parent]->index = parent;
		h->elements[child]->index = child;

		child = parent;
	}
}

void *
heap_extract(struct Heap * const h)
{
	if (h->n_elements == 0) {
		return NULL;
	}

	struct HeapElement * const he = h->elements[0];
	h->elements[0] = h->elements[h->n_elements - 1];
	h->elements[0]->index = 0;
	h->n_elements--;

	sift_down(h);

	void * const data = he->data;
	free(he);
	return data;
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

		if (h->elements[swap]->priority > h->elements[left]->priority) {
			swap = left;
		}

		size_t const right = left+1;
		if (right <= end &&
				h->elements[swap]->priority > h->elements[right]->priority) {
			swap = right;
		}

		if (swap == root) {
			return;
		}

		struct HeapElement * const e = h->elements[root];
		h->elements[root] = h->elements[swap];
		h->elements[swap] = e;

		h->elements[root]->index = root;
		h->elements[swap]->index = swap;
		root = swap;
	}
}

void
heap_decrease_priority(struct Heap * const h,
		struct HeapElement * const he,
		int64_t const priority)
{
	he->priority = priority;
	sift_up(h, he->index);
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

	// Test decreasing priority.

	h = heap_create(10);
	assert(h != NULL);

	struct HeapElement * he0 = heap_insert(h, 3, &mydata3);
	assert(he0->index == 0);

	struct HeapElement * he1 = heap_insert(h, 2, &mydata2);
	assert(he1->index == 0);
	assert(he0->index == 1);

	heap_decrease_priority(h, he0, 1);
	assert(he0->index == 0);
	assert(he1->index == 1);

	heap_free(h);

	return 0;
}

#endif
