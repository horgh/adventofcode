#include <assert.h>
#include <errno.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Position {
	int x;
	int y;
	int steps;
};

static bool __is_open(const int, const int, const int);
static int __solve_bfs(const int, const int, const int);
static char * __position_to_string(const struct Position * const);
static bool __add_neighbours(
		const int, struct Queue * const, const struct Position * const);
static bool __enqueue(struct Queue * const, const int, const int, const int);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 4) {
		printf(
				"Usage: %s <input number> <destination x> <destination y>\n", argv[0]);
		return 1;
	}
	const int fav_number = atoi(argv[1]);
	const int dest_x = atoi(argv[2]);
	const int dest_y = atoi(argv[3]);

	assert(__is_open(7, 4, 10));
	assert(!__is_open(8, 4, 10));

	const int steps = __solve_bfs(fav_number, dest_x, dest_y);
	printf("%d\n", steps);

	return 0;
}

static bool
__is_open(const int x, const int y, const int fav_number)
{
	const int n = x * x + 3 * x + 2 * x * y + y + y * y + fav_number;

	return __builtin_popcount((unsigned int)n) % 2 == 0;
}

static int
__solve_bfs(const int fav_number, const int dest_x, const int dest_y)
{
	struct Queue * queue = calloc(1, sizeof(struct Queue));
	if (!queue) {
		printf("%s\n", strerror(errno));
		return -1;
	}

	// Start at (1, 1)
	if (!__enqueue(queue, 1, 1, 0)) {
		printf("enqueue()\n");
		destroy_queue(queue, free);
		return -1;
	}

	struct htable * const visited = hash_init(1000000);
	if (!visited) {
		printf("hash_init\n");
		destroy_queue(queue, free);
		return -1;
	}

	while (queue->first) {
		struct Position * const pos = dequeue(queue);
		if (!pos) {
			printf("dequeue()\n");
			destroy_queue(queue, free);
			return -1;
		}

		if (pos->x == dest_x && pos->y == dest_y) {
			destroy_queue(queue, free);
			hash_free(visited, NULL);
			const int steps = pos->steps;
			free(pos);
			return steps;
		}

		char * const pos_str = __position_to_string(pos);
		if (!pos_str) {
			printf("__position_to_string\n");
			destroy_queue(queue, free);
			hash_free(visited, NULL);
			free(pos);
			return -1;
		}

		if (hash_has_key(visited, pos_str)) {
			free(pos);
			free(pos_str);
			continue;
		}

		if (!hash_set(visited, pos_str, NULL)) {
			printf("hash_set\n");
			destroy_queue(queue, free);
			hash_free(visited, NULL);
			free(pos);
			free(pos_str);
			return -1;
		}

		if (!__add_neighbours(fav_number, queue, pos)) {
			printf("__add_neighbours\n");
			destroy_queue(queue, free);
			hash_free(visited, NULL);
			free(pos);
			return -1;
		}

		free(pos);
		free(pos_str);
	}

	destroy_queue(queue, free);
	hash_free(visited, NULL);
	return -1;
}

static char *
__position_to_string(const struct Position * const pos)
{
	if (!pos) {
		printf("%s\n", strerror(EINVAL));
		return NULL;
	}

	char * const s = calloc(128, sizeof(char));
	if (!s) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	snprintf(s, 128, "%d,%d", pos->x, pos->y);

	return s;
}

static bool
__add_neighbours(const int fav_number,
		struct Queue * const queue,
		const struct Position * const current_pos)
{
	if (!queue || !current_pos) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	// Up
	if (__is_open(current_pos->x, current_pos->y + 1, fav_number)) {
		if (!__enqueue(queue,
						current_pos->x,
						current_pos->y + 1,
						current_pos->steps + 1)) {
			printf("__enqueue()\n");
			return false;
		}
	}

	// Down
	if (__is_open(current_pos->x, current_pos->y - 1, fav_number)) {
		if (!__enqueue(queue,
						current_pos->x,
						current_pos->y - 1,
						current_pos->steps + 1)) {
			printf("__enqueue()\n");
			return false;
		}
	}

	// Right
	if (__is_open(current_pos->x + 1, current_pos->y, fav_number)) {
		if (!__enqueue(queue,
						current_pos->x + 1,
						current_pos->y,
						current_pos->steps + 1)) {
			printf("__enqueue()\n");
			return false;
		}
	}

	// Left
	if (__is_open(current_pos->x - 1, current_pos->y, fav_number)) {
		if (!__enqueue(queue,
						current_pos->x - 1,
						current_pos->y,
						current_pos->steps + 1)) {
			printf("__enqueue()\n");
			return false;
		}
	}

	return true;
}

static bool
__enqueue(struct Queue * const queue, const int x, const int y, const int steps)
{
	if (!queue) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	struct Position * const pos = calloc(1, sizeof(struct Position));
	if (!pos) {
		printf("%s\n", strerror(errno));
		return false;
	}

	pos->x = x;
	pos->y = y;
	pos->steps = steps;

	if (!enqueue(queue, pos)) {
		printf("enqueue()\n");
		free(pos);
		return false;
	}

	return true;
}
