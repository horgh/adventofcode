#ifndef _MAP_H
#define _MAP_H

#include <stdbool.h>
#include <stddef.h>

struct hnode;

struct hnode {
	char * key;
	int value;
	struct hnode * next;
};

struct htable {
	size_t size;
	struct hnode * * nodes;
};

struct htable *
hash_init(const size_t);

int
hasher(const char * const, const size_t);

bool
hash_set(const struct htable * const, const char * const, const int);

int
hash_get(const struct htable * const, const char * const);

bool
hash_delete(const struct htable * const, const char * const);

bool
hash_iterate(const struct htable * const,
		void * (const struct hnode * const));

bool
hash_free(struct htable *);

#endif
