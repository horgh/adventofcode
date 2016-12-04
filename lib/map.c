// for strdup
#define _XOPEN_SOURCE 500

#include "map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

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

	return hash%(int)n;
}

bool
hash_set(const struct htable * const h, const char * const key, const int value)
{
	if (!h || !key || strlen(key) == 0) {
		return false;
	}

	int hash = hasher(key, h->size);

	struct hnode * nptr = h->nodes[hash];

	if (!nptr) {
		nptr = calloc(1, sizeof(struct hnode));
		if (!nptr) {
			return false;
		}

		nptr->key = strdup(key);
		if (!nptr->key) {
			free(nptr);
			return false;
		}

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

	nptr->key = strdup(key);
	if (!nptr->key) {
		free(nptr);
		return false;
	}

	nptr->value = value;

	prev->next = nptr;
	return true;
}

int
hash_get(const struct htable * const h, const char * const key)
{
	if (!h || !key || strlen(key) == 0) {
		return -1;
	}

	int hash = hasher(key, h->size);

	struct hnode * nptr = h->nodes[hash];

	while (nptr) {
		if (strcmp(nptr->key, key) == 0) {
			return nptr->value;
		}

		nptr = nptr->next;
	}

	return -1;
}

bool
hash_delete(const struct htable * const h, const char * const key)
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
				free(nptr);
				return true;
			}

			h->nodes[hash] = nptr->next;
			free(nptr->key);
			free(nptr);
			return true;
		}

		prev = nptr;
		nptr = nptr->next;
	}

	return false;
}

bool
hash_iterate(const struct htable * const h,
		void * fn(const struct hnode * const))
{
	if (!h || !fn) {
		return false;
	}

	for (size_t i = 0; i < h->size; i++) {
		const struct hnode * nptr = *(h->nodes+i);

		while (nptr) {
			fn(nptr);
			nptr = nptr->next;
		}
	}

	return true;
}

bool
hash_free(struct htable * h)
{
	if (!h) {
		return false;
	}

	for (size_t i = 0; i < h->size; i++) {
		struct hnode * nptr = *(h->nodes+i);

		while (nptr) {
			struct hnode * next = nptr->next;
			free(nptr->key);
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

int
main(int argc, char ** argv)
{
	(void) argc;
	(void) argv;

	struct htable * h = NULL;

	size_t size = 100;

	int found_value = 0;

	h = hash_init(size);
	assert(h != NULL);

	const char * key1 = "blah";
	int value1 = 5;
	assert(hash_set(h, key1, value1));

	found_value = hash_get(h, key1);
	printf("found %d\n", found_value);
	assert(found_value == value1);

	const char * key2 = "blah2";
	int value2 = 10;
	assert(hash_set(h, key2, value2));

	found_value = hash_get(h, key2);
	printf("found %d\n", found_value);
	assert(found_value == value2);

	found_value = hash_get(h, key1);
	printf("found %d\n", found_value);
	assert(found_value == value1);

	assert(hash_free(h));
}

#endif
