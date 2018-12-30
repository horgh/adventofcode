#include <assert.h>
#include <errno.h>
#include <hash.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MOVES 1024

struct Map {
	int x;
	int y;
	char * moves;
};

static char * __solve(const char * const);
static bool __enqueue(struct Queue * const,
		const int,
		const int,
		const char * const,
		const char * const);
static void __destroy_map(void * const);
static bool __add_neighbours(
		const char * const, struct Queue * const, struct Map * const);
static bool __is_up_open(const unsigned char * const);
static bool __is_down_open(const unsigned char * const);
static bool __is_left_open(const unsigned char * const);
static bool __is_right_open(const unsigned char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input>\n", argv[0]);
		return 1;
	}
	const char * const input = argv[1];

	char * const test0 = __solve("ihgpwlah");
	assert(test0 != NULL);
	assert(strlen(test0) == 370);
	free(test0);
	// printf("test0 done\n");

	char * const test1 = __solve("kglvqrro");
	assert(test1 != NULL);
	assert(strlen(test1) == 492);
	free(test1);
	// printf("test1 done\n");

	char * const test2 = __solve("ulqzkmiv");
	assert(test2 != NULL);
	assert(strlen(test2) == 830);
	free(test2);
	// printf("test2 done\n");

	char * const path = __solve(input);
	if (path) {
		// printf("%s\n", path);
		printf("%zu\n", strlen(path));
		free(path);
	}

	return 0;
}

static char *
__solve(const char * const input)
{
	struct Queue * const q = calloc(1, sizeof(struct Queue));
	if (!q) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	if (!__enqueue(q, 0, 0, "", "")) {
		printf("__enqueue\n");
		destroy_queue(q, __destroy_map);
		return NULL;
	}

	char * longest_solution = calloc(MAX_MOVES + 1, sizeof(char));
	if (!longest_solution) {
		printf("%s\n", strerror(errno));
		destroy_queue(q, __destroy_map);
		return NULL;
	}

	while (q->first) {
		struct Map * const current_map = dequeue(q);
		if (!current_map) {
			printf("dequeue\n");
			destroy_queue(q, __destroy_map);
			return NULL;
		}

		if (current_map->x == 3 && current_map->y == 3) {
			memset(longest_solution, 0, MAX_MOVES + 1);
			strcat(longest_solution, current_map->moves);
			__destroy_map(current_map);
			continue;
		}

		if (!__add_neighbours(input, q, current_map)) {
			printf("__add_neighbours\n");
			destroy_queue(q, __destroy_map);
			__destroy_map(current_map);
			return NULL;
		}

		__destroy_map(current_map);
	}

	destroy_queue(q, __destroy_map);

	return longest_solution;
}

static bool
__enqueue(struct Queue * const q,
		const int x,
		const int y,
		const char * const moves,
		const char * const move)
{
	if (strlen(moves) == MAX_MOVES) {
		printf("too many moves\n");
		return false;
	}

	struct Map * const m = calloc(1, sizeof(struct Map));
	if (!m) {
		printf("%s\n", strerror(errno));
		return false;
	}

	m->x = x;
	m->y = y;
	m->moves = calloc(MAX_MOVES + 1, sizeof(char));
	if (!m->moves) {
		printf("%s\n", strerror(errno));
		return false;
	}

	strcat(m->moves, moves);
	strcat(m->moves, move);

	if (!enqueue(q, m)) {
		printf("enqueue\n");
		__destroy_map(m);
		return false;
	}

	return true;
}

static void
__destroy_map(void * const p)
{
	if (!p) {
		return;
	}

	struct Map * const m = p;

	if (m->moves) {
		free(m->moves);
	}

	free(m);
}

static bool
__add_neighbours(const char * const input,
		struct Queue * const q,
		struct Map * const current_map)
{
	char * const key = calloc(strlen(input) + MAX_MOVES + 1, sizeof(char));
	if (!key) {
		printf("%s\n", strerror(errno));
		return false;
	}
	strcat(key, input);
	strcat(key, current_map->moves);

	unsigned char * const hash = md5(key);
	if (!hash) {
		printf("hash\n");
		free(key);
		return false;
	}
	free(key);

	if (current_map->x > 0 && __is_up_open(hash)) {
		if (!__enqueue(
						q, current_map->x - 1, current_map->y, current_map->moves, "U")) {
			printf("__enqueue\n");
			free(hash);
			return false;
		}
	}

	if (current_map->x < 3 && __is_down_open(hash)) {
		if (!__enqueue(
						q, current_map->x + 1, current_map->y, current_map->moves, "D")) {
			printf("__enqueue\n");
			free(hash);
			return false;
		}
	}

	if (current_map->y > 0 && __is_left_open(hash)) {
		if (!__enqueue(
						q, current_map->x, current_map->y - 1, current_map->moves, "L")) {
			printf("__enqueue\n");
			free(hash);
			return false;
		}
	}

	if (current_map->y < 3 && __is_right_open(hash)) {
		if (!__enqueue(
						q, current_map->x, current_map->y + 1, current_map->moves, "R")) {
			printf("__enqueue\n");
			free(hash);
			return false;
		}
	}

	free(hash);

	return true;
}

static bool
__is_up_open(const unsigned char * const hash)
{
	const uint8_t x0 = hash[0] >> 4;
	return x0 == 0x0b || x0 == 0x0c || x0 == 0x0d || x0 == 0x0e || x0 == 0x0f;
}

static bool
__is_down_open(const unsigned char * const hash)
{
	const uint8_t x0 = (uint8_t)(hash[0] << 4);
	return x0 == 0xb0 || x0 == 0xc0 || x0 == 0xd0 || x0 == 0xe0 || x0 == 0xf0;
}

static bool
__is_left_open(const unsigned char * const hash)
{
	const uint8_t x0 = hash[1] >> 4;
	return x0 == 0x0b || x0 == 0x0c || x0 == 0x0d || x0 == 0x0e || x0 == 0x0f;
}

static bool
__is_right_open(const unsigned char * const hash)
{
	const uint8_t x0 = (uint8_t)(hash[1] << 4);
	return x0 == 0xb0 || x0 == 0xc0 || x0 == 0xd0 || x0 == 0xe0 || x0 == 0xf0;
}
