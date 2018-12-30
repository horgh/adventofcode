#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>

struct QueueElement;

struct QueueElement {
	void * data;
	struct QueueElement * next;
	struct QueueElement * previous;
};

struct Queue {
	struct QueueElement * first;
	struct QueueElement * last;
};

bool enqueue(struct Queue * const, void * const);

void * dequeue(struct Queue * const);

void destroy_queue(struct Queue *, void(void * const));

#endif
