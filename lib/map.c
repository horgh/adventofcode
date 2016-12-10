// for strdup
#define _XOPEN_SOURCE 500

#include "map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

static bool
__hash_set(const struct htable * const, const int,
		const void * const, const size_t, void * const);

struct htable *
hash_init(const size_t n)
{
	struct htable * h = calloc(1, sizeof(struct htable));
	if (!h)  {
		return NULL;
	}

	h->size = n;

	h->nodes = calloc(n, sizeof(struct htable *));
	if (!h->nodes) {
		free(h);
		return NULL;
	}

	return h;
}

__attribute__((pure))
int
hasher(const char * const key, const size_t n)
{
	if (!key || strlen(key) == 0) {
		return 0;
	}

	int hash = 0;
	const int mult = 31;

	for (size_t i = 0; i < strlen(key); i++) {
		hash += mult*hash+key[i];
	}
	hash = abs(hash);

	int actual_hash = hash%(int)n;

#ifdef TEST_MAP
	printf("%s hashes to %d\n", key, actual_hash);
#endif

	return actual_hash;
}

bool
hash_set(const struct htable * const h, const char * const key,
		void * const value)
{
	if (!h || !key || strlen(key) == 0) {
		return false;
	}

	int hash = hasher(key, h->size);

	return __hash_set(h, hash, key, strlen(key)+1, value);
}

bool
hash_set_i(const struct htable * const h, const int key, void * const value)
{
	if (!h) {
		return false;
	}

	int hash = abs(key) % (int) h->size;

	return __hash_set(h, hash, &key, sizeof(key), value);
}

static bool
__hash_set(const struct htable * const h, const int hash,
		const void * const key, const size_t key_size, void * const value)
{
	if (!h || !key) {
		return false;
	}

	struct hnode * nptr = h->nodes[hash];

	if (!nptr) {
		nptr = calloc(1, sizeof(struct hnode));
		if (!nptr) {
			return false;
		}

		nptr->key = calloc(1, key_size);
		if (!nptr->key) {
			free(nptr);
			return false;
		}
		memcpy(nptr->key, key, key_size);

		nptr->key_size = key_size;

		nptr->value = value;

		h->nodes[hash] = nptr;
		return true;
	}

	struct hnode * prev = NULL;

	while (nptr) {
		if (strcmp(nptr->key, key) == 0) {
			nptr->value = value;
			return true;
		}

		prev = nptr;
		nptr = nptr->next;
	}

	nptr = calloc(1, sizeof(struct hnode));
	if (!nptr) {
		return false;
	}

	nptr->key = calloc(1, key_size);
	if (!nptr->key) {
		free(nptr);
		return false;
	}
	memcpy(nptr->key, key, key_size);

	nptr->key_size = key_size;

	nptr->value = value;

	prev->next = nptr;
	return true;
}

__attribute__((pure))
void *
hash_get(const struct htable * const h, const char * const key)
{
	if (!h || !key || strlen(key) == 0) {
		return NULL;
	}

	int hash = hasher(key, h->size);

	struct hnode * nptr = h->nodes[hash];

	while (nptr) {
		if (strcmp(nptr->key, key) == 0) {
			return nptr->value;
		}

		nptr = nptr->next;
	}

	return NULL;
}

bool
hash_delete(const struct htable * const h, const char * const key,
		void fn(void * const))
{
	if (!h || !key || strlen(key) == 0) {
		return false;
	}

	int hash = hasher(key, h->size);

	struct hnode * nptr = *(h->nodes+hash);

	if (!nptr) {
		return false;
	}

	struct hnode * prev = NULL;

	while (nptr) {
		if (strcmp(nptr->key, key) == 0) {
			if (prev) {
				prev->next = nptr->next;
				free(nptr->key);
				fn(nptr->value);
				free(nptr);
				return true;
			}

			h->nodes[hash] = nptr->next;
			free(nptr->key);
			fn(nptr->value);
			free(nptr);
			return true;
		}

		prev = nptr;
		nptr = nptr->next;
	}

	return false;
}

void * *
hash_get_keys(const struct htable * const h)
{
	if (!h) {
		return NULL;
	}

	size_t array_size = h->size;

	void * * keys = calloc(array_size, sizeof(void *));
	if (!keys) {
		return NULL;
	}

	size_t keys_i = 0;

	for (size_t i = 0; i < h->size; i++) {
		const struct hnode * nptr = *(h->nodes+i);

		while (nptr) {
			// Terminate with null at end always.
			if (keys_i == array_size-1) {
				size_t new_array_size = array_size*2;

				void * * const new_keys = realloc(keys, new_array_size);
				if (!new_keys) {
					free(keys);
					return NULL;
				}

				memset(new_keys+array_size, 0, array_size);

				array_size = new_array_size;
				keys = new_keys;
			}

			keys[keys_i] = nptr->key;
			keys_i++;

			nptr = nptr->next;
		}
	}

	return keys;
}

// p gets passed to each node. You can use it to carry around state.
bool
hash_iterate(const struct htable * const h,
		void fn(const struct hnode * const, void * const), void * const p)
{
	if (!h || !fn) {
		return false;
	}

	for (size_t i = 0; i < h->size; i++) {
		const struct hnode * nptr = *(h->nodes+i);

		while (nptr) {
			fn(nptr, p);
			nptr = nptr->next;
		}
	}

	return true;
}

bool
hash_free(struct htable * h, void fn(void * const))
{
	if (!h) {
		return false;
	}

	for (size_t i = 0; i < h->size; i++) {
		struct hnode * nptr = *(h->nodes+i);

		while (nptr) {
			struct hnode * next = nptr->next;
			free(nptr->key);
			fn(nptr->value);
			free(nptr);
			nptr = next;
		}
	}

	free(h->nodes);
	free(h);
	return true;
}

#ifdef TEST_MAP

#include <assert.h>

static void
__get_value(const struct hnode * const, void * const);

int
main(int argc, char ** argv)
{
	(void) argc;
	(void) argv;

	struct htable * h = NULL;

	size_t size = 100;

	void * found_value = NULL;
	int found_int = 0;

	h = hash_init(size);
	assert(h != NULL);


	// blah => 5

	const char * key1 = "blah";
	int * value1 = calloc(1, sizeof(int));
	assert(value1 != NULL);
	*value1 = 5;
	assert(hash_set(h, key1, value1));

	found_value = hash_get(h, key1);
	found_int = * (int *) found_value;
	printf("found %d\n", found_int);
	assert(found_int == *value1);


	// blah2 => 10

	const char * key2 = "blah2";
	int * value2 = calloc(1, sizeof(int));
	assert(value2 != NULL);
	*value2 = 10;
	assert(hash_set(h, key2, value2));

	found_value = hash_get(h, key2);
	found_int = * (int *) found_value;
	printf("found %d\n", found_int);
	assert(found_int == *value2);

	found_value = hash_get(h, key1);
	found_int = * (int *) found_value;
	printf("found %d\n", found_int);
	assert(found_int == *value1);


	// Test hash_iterate
	int i = -1;
	hash_iterate(h, __get_value, &i);
	// 10 happens to have highest hash.
	assert(i == 10);


	// Test hash_get_keys
	void * * keys = hash_get_keys(h);

	for (size_t j = 0; keys[j]; j++) {
		const char * const key = keys[j];
		printf("key: %s\n", key);
	}

	free(keys);

	assert(hash_free(h, free));
}

static void
__get_value(const struct hnode * const node, void * const p)
{
	int * value = node->value;

	int * ret = p;

	*ret = *value;
}

#endif
