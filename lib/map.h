#ifndef _MAP_H
#define _MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct hnode;

struct hnode {
	char * key;
	size_t key_size;

	void * value;

	struct hnode * next;
};

struct htable {
	size_t size;
	struct hnode * * nodes;
	size_t collisions;
};

struct htable *
hash_init(const size_t);

struct htable *
hash_copy(const struct htable * const);

bool
hash_set(struct htable * const, const char * const,
		void * const);

bool
hash_set_i(struct htable * const, const int, void * const);

void *
hash_get(const struct htable * const, const char * const);

bool
hash_has_key(const struct htable * const, const char * const);

bool
hash_delete(struct htable * const, const char * const,
		void (void * const));

void * *
hash_get_keys(const struct htable * const);

void
hash_free_keys(void * * const);

bool
hash_iterate(const struct htable * const,
		void (const struct hnode * const, void * const), void * const);

int
hash_count_elements(const struct htable * const);

bool
hash_free(struct htable *, void (void * const));

#endif
