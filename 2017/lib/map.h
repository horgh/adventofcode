#ifndef _MAP_H
#define _MAP_H

#define _POSIX_C_SOURCE 200809L

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
hash_init(size_t const);

struct htable *
hash_copy(struct htable const * const);

bool
hash_set(struct htable * const, char const * const,
		void * const);

bool
hash_set_i(struct htable * const, int const, void * const);

void *
hash_get(const struct htable * const, char const * const);

void *
hash_get_i(struct htable const * const, int const);

bool
hash_has_key(struct htable const * const, char const * const);

bool
hash_has_key_i(struct htable const * const, int const);

bool
hash_delete(struct htable * const, char const * const,
		void (void *));

bool
hash_delete_i(struct htable * const, int const, void (void * const));

void * *
hash_get_keys(struct htable const * const);

void
hash_free_keys(void * * const);

bool
hash_iterate(struct htable const * const,
		void (struct hnode const * const, void * const), void * const);

int
hash_count_elements(struct htable const * const);

bool
hash_free(struct htable * const, void (void * const));

#endif
