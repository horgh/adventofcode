#include "queue.h"
#include <stdbool.h>
#include <stdlib.h>

static void
__destroy_queue_element(struct QueueElement *, void (void * const));

bool
enqueue(struct Queue * const queue, void * const data)
{
	struct QueueElement * const qe = calloc(1, sizeof(struct QueueElement));
	if (!qe) {
		return false;
	}

	qe->data = data;

	if (!queue->first) {
		queue->first = qe;
		queue->last = qe;
		return true;
	}

	queue->last->next = qe;
	qe->previous = queue->last;
	queue->last = qe;
	return true;
}

void *
dequeue(struct Queue * const queue)
{
	if (!queue->first) {
		return NULL;
	}

	struct QueueElement * const qe = queue->first;

	// This is only element.
	if (qe->next == NULL) {
		queue->first = NULL;
		queue->last = NULL;
		void * const data = qe->data;
		free(qe);
		return data;
	}

	// There are more.
	queue->first = qe->next;
	queue->first->previous = NULL;

	void * const data = qe->data;
	free(qe);
	return data;
}

void
destroy_queue(struct Queue * queue, void fn(void * const))
{
	if (!queue) {
		return;
	}

	struct QueueElement * qe = queue->first;
	while (qe) {
		struct QueueElement * const qn = qe->next;
		__destroy_queue_element(qe, fn);
		qe = qn;
	}

	free(queue);
}

static void
__destroy_queue_element(struct QueueElement * qe, void fn(void * const))
{
	if (!qe) {
		return;
	}

	if (qe->data) {
		if (fn) {
			fn(qe->data);
		}
	}

	free(qe);
}

#ifdef TEST_QUEUE

#include <assert.h>
#include <stdio.h>

int main(void)
{
	struct Queue * q = NULL;
	int * v = NULL;


	// One element.

	q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);

	v = calloc(1, sizeof(int));
	assert(v != NULL);
	*v = 5;

	assert(enqueue(q, v));

	v = dequeue(q);
	assert(v != NULL);
	assert(*v == 5);
	free(v);

	v = dequeue(q);
	assert(v == NULL);

	destroy_queue(q, free);


	// Two elements.

	q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);

	v = calloc(1, sizeof(int));
	assert(v != NULL);
	*v = 5;

	assert(enqueue(q, v));

	v = calloc(1, sizeof(int));
	assert(v != NULL);
	*v = 123;

	assert(enqueue(q, v));

	v = dequeue(q);
	assert(v != NULL);
	assert(*v == 5);
	free(v);

	v = dequeue(q);
	assert(v != NULL);
	assert(*v == 123);
	free(v);

	destroy_queue(q, free);


	// Two elements. Enqueue but don't dequeue. Test destruction with elements.
	q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);

	v = calloc(1, sizeof(int));
	assert(v != NULL);
	*v = 5;

	assert(enqueue(q, v));

	v = calloc(1, sizeof(int));
	assert(v != NULL);
	*v = 123;

	assert(enqueue(q, v));

	destroy_queue(q, free);


	return 0;
}

#endif
