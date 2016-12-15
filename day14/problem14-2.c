#include <assert.h>
#include <errno.h>
#include <hash.h>
#include <map.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *
__generate_key(const char * const, const size_t, size_t * const);
static bool
__hash_repeats(const unsigned char * const, size_t, uint8_t * const);
static bool
__hash_repeats_in_1000(const char * const, const size_t,
		const uint8_t);
static bool
__repeats_5_times(const uint8_t * const, const size_t,
		const uint8_t);
static unsigned char *
__stretched_md5(const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input>\n", argv[0]);
		return 1;
	}
	const char * const input = argv[1];

	uint8_t a[4];
	memset(a, 0, 4*sizeof(uint8_t));
	uint8_t t = 0;
	a[0] = 0xff;
	a[1] = 0xf0;
	assert(__hash_repeats(a, 2, &t));
	assert(t == 0x0f);

	a[0] = 0x0f;
	a[1] = 0xff;
	assert(__hash_repeats(a, 2, &t));
	assert(t == 0x0f);

	a[0] = 0xff;
	a[1] = 0xff;
	a[2] = 0xf0;
	assert(__repeats_5_times(a, 3, 0x0f));

	const size_t hash_sz = 16;
	size_t current_index = 0;

	for (size_t i = 0; i < 64; i++) {
		printf("generating key %zu...\n", i+1);
		unsigned char * const key = __generate_key(input, current_index,
				&current_index);
		printf("key %zu (index %zu): ", i, current_index);
		current_index++;
		for (size_t j = 0; j < hash_sz; j++) {
			printf("%02x", key[j]);
		}
		printf("\n");
		free(key);
	}

	return 0;
}

static unsigned char *
__generate_key(const char * const input, const size_t index,
		size_t * const current_index)
{
	const size_t plain_sz = 1024;
	char * const plain = calloc(plain_sz, sizeof(char));
	if (!plain) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	const size_t hash_sz = 16;

	size_t i = index;

	while (1) {
		memset(plain, 0, plain_sz);
		snprintf(plain, plain_sz, "%s%zu", input, i);

		unsigned char * const hash = __stretched_md5(plain);
		if (!hash) {
			printf("__stretched_md5\n");
			free(plain);
			return NULL;
		}

		// Find hash with nybble that repeats 3x. And find the nybble.
		uint8_t repeater = 0;
		if (!__hash_repeats(hash, hash_sz, &repeater)) {
			free(hash);
			i++;
			continue;
		}

		// If it repeats this nybble 5x in the next 1000, it's a key.
		if (__hash_repeats_in_1000(input, i, repeater)) {
			free(plain);
			*current_index = i;
			return hash;
		}

		free(hash);
		i++;
	}

	free(plain);
	return NULL;
}

static bool
__hash_repeats(const uint8_t * const hash, size_t hash_len, uint8_t * const r)
{
	for (size_t i = 0; i < hash_len-1; i++) {
		const uint8_t x0 = hash[i] >> 4;
		const uint8_t x1 = (uint8_t) (hash[i] << 4) >> 4;
		const uint8_t x2 = hash[i+1] >> 4;
		const uint8_t x3 = (uint8_t) (hash[i+1] << 4) >> 4;

		if (x0 == x1 && x0 == x2) {
			*r = x0;
			return true;
		}

		if (x1 == x2 && x1 == x3) {
			*r = x1;
			return true;
		}
	}

	return false;
}

static bool
__hash_repeats_in_1000(const char * const input, const size_t index,
		const uint8_t repeater)
{
	const size_t plain_sz = 1024;
	char * const plain = calloc(plain_sz, sizeof(char));
	if (!plain) {
		printf("%s\n", strerror(errno));
		return false;
	}

	const size_t hash_sz = 16;

	for (size_t i = index+1; i < index+1+1000; i++) {
		memset(plain, 0, plain_sz);
		snprintf(plain, plain_sz, "%s%zu", input, i);

		unsigned char * const hash = __stretched_md5(plain);
		if (!hash) {
			printf("__stretched_md5\n");
			free(plain);
			return false;
		}

		if (__repeats_5_times(hash, hash_sz, repeater)) {
			free(plain);
			free(hash);
			return true;
		}

		free(hash);
	}

	free(plain);
	return false;
}

static bool
__repeats_5_times(const uint8_t * const hash, const size_t hash_sz,
		const uint8_t t)
{
	for (size_t i = 0; i < hash_sz-2; i++) {
		const uint8_t x0 = hash[i] >> 4;
		const uint8_t x1 = (uint8_t) (hash[i] << 4) >> 4;

		const uint8_t x2 = hash[i+1] >> 4;
		const uint8_t x3 = (uint8_t) (hash[i+1] << 4) >> 4;

		const uint8_t x4 = hash[i+2] >> 4;
		const uint8_t x5 = (uint8_t) (hash[i+2] << 4) >> 4;

		if (x0 == t && x1 == t && x2 == t && x3 == t && x4 == t) {
			return true;
		}

		if (x1 == t && x2 == t && x3 == t && x4 == t && x5 == t) {
			return true;
		}
	}

	return false;
}

static unsigned char *
__stretched_md5(const char * const s)
{
	// Store computed stretched hash for s in hashes map.

	static struct htable * hashes = NULL;
	if (!hashes) {
		hashes = hash_init(1000000);
		if (!hashes) {
			printf("hash_init\n");
			return NULL;
		}
	}

	const size_t hash_sz = 16;

	if (hash_has_key(hashes, s)) {
		void * const d = hash_get(hashes, s);
		unsigned char * const stretched = d;

		// Return a copy as the caller frees the memory
		unsigned char * hash_cp = calloc(hash_sz, sizeof(unsigned char));
		if (!hash_cp) {
			printf("%s\n", strerror(errno));
			return NULL;
		}
		memcpy(hash_cp, stretched, hash_sz);

		return hash_cp;
	}

	unsigned char * hash = md5(s);
	if (!hash) {
		printf("md5\n");
		return NULL;
	}

	size_t plain_sz = 33;
	char * const plain = calloc(33, sizeof(char));
	if (!plain) {
		printf("%s\n", strerror(errno));
		free(hash);
		return NULL;
	}

	for (size_t i = 0; i < 2016; i++) {
		memset(plain, 0, plain_sz);
		for (size_t j = 0; j < hash_sz; j++) {
			sprintf(plain+j*2, "%02x", hash[j]);
		}

		unsigned char * const new_hash = md5(plain);
		if (!new_hash) {
			printf("%s\n", strerror(errno));
			free(hash);
			free(plain);
			return NULL;
		}

		free(hash);
		hash = new_hash;
	}

	free(plain);

	if (!hash_set(hashes, s, hash)) {
		printf("hash_set\n");
		free(hash);
		return NULL;
	}

	// Return a copy as the caller frees the memory
	unsigned char * hash_cp = calloc(hash_sz, sizeof(unsigned char));
	if (!hash_cp) {
		printf("%s\n", strerror(errno));
		free(hash);
		return NULL;
	}
	memcpy(hash_cp, hash, hash_sz);

	return hash_cp;
}
