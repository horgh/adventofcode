#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WIDTH 256
#define MAX_HEIGHT 64
#define MAX_TARGETS 100
#define HASH_SIZE 102400

struct Position {
	int x;
	int y;
	int steps;
};

struct Target {
	int x;
	int y;
	bool visited;
};

enum Location {WALL, OPEN, TARGET};

static void
__destroy_locations(enum Location * * const);
static int
__shortest_tour(enum Location * * const,
		struct Target * const, const size_t, const int,
		const int);
static int
__permute_targets(enum Location * * const,
		struct Target * const, const size_t, const int,
		const int, struct Target * const,
		const size_t);
static int
__tour(enum Location * * const, struct Target * const,
		const size_t, const int, const int);
static int
__shortest_path(enum Location * * const, const int,
		const int, const int, const int);
static bool
__enqueue(struct Queue * const, const int, const int, const int);
static bool
__add_neighbours(enum Location * * const, struct Queue * const,
		const struct Position * const, const int);

int main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	enum Location * * const locations = calloc(MAX_WIDTH,
			sizeof(enum Location *));
	if (!locations) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	for (size_t i = 0; i < MAX_WIDTH; i++) {
		locations[i] = calloc(MAX_HEIGHT, sizeof(enum Location));
		if (!locations[i]) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_locations(locations);
			return 1;
		}
	}

	struct Target * const targets = calloc(MAX_TARGETS, sizeof(struct Target));
	if (!targets) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		__destroy_locations(locations);
		return 1;
	}

	size_t height = 0;
	size_t width = 0;

	size_t targets_i = 0;

	int startx = -1;
	int starty = -1;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		for (size_t i = 0; i < strlen(buf); i++) {
			if (buf[i] == '#') {
				locations[i][height] = WALL;
				continue;
			}

			if (buf[i] == '.') {
				locations[i][height] = OPEN;
				continue;
			}

			if (isdigit(buf[i])) {
				locations[i][height] = TARGET;
				if (buf[i] == '0') {
					startx = (int) i;
					starty = (int) height;
				} else {
					targets[targets_i].x = (int) i;
					targets[targets_i].y = (int) height;
					printf("found target %zu at %d,%d\n", targets_i, (int) i,
							(int) height);
					targets_i++;
				}
				continue;
			}

			if (buf[i] == '\n') {
				width = i;
				continue;
			}

			printf("unknown point: %zu: %s\n", i, buf);
			fclose(fh);
			__destroy_locations(locations);
			free(targets);
			return 1;
		}

		height++;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_locations(locations);
		free(targets);
		return 1;
	}

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			if (locations[x][y] == WALL) {
				printf("W");
			} else if (locations[x][y] == TARGET) {
				printf("T");
			} else {
				printf("O");
			}
		}
		printf("\n");
	}

	printf("starting position is %d,%d\n", startx, starty);
	const int steps = __shortest_tour(locations, targets, targets_i, startx,
			starty);
	printf("%d\n", steps);

	__destroy_locations(locations);
	free(targets);

	return 0;
}

static void
__destroy_locations(enum Location * * const locations)
{
	if (!locations) {
		return;
	}

	for (size_t x = 0; x < MAX_WIDTH; x++) {
		free(locations[x]);
	}

	free(locations);
}

static int
__shortest_tour(enum Location * * const locations,
		struct Target * const targets, const size_t targets_sz, const int startx,
		const int starty)
{
	struct Target * current_targets = calloc(targets_sz, sizeof(struct Target));
	if (!current_targets) {
		printf("%s\n", strerror(errno));
		return -1;
	}

	// Optimization: Record shortest paths from x0,y0 to x1,y1 and check whether
	// we've calculated it previously.

	const int steps = __permute_targets(locations, targets, targets_sz,
			startx, starty, current_targets, 0);
	free(current_targets);
	return steps;
}

// Permute by removing from targets and adding to current_targets.
static int
__permute_targets(enum Location * * const locations,
		struct Target * const targets, const size_t targets_sz, const int startx,
		const int starty, struct Target * const current_targets,
		const size_t current_targets_sz)
{
	if (targets_sz == 0) {
#ifdef DEBUG
		printf("starting tour from %d,%d (%zu targets)\n", startx, starty,
				current_targets_sz);
#endif
		return __tour(locations, current_targets, current_targets_sz, startx,
				starty);
	}

	int steps = INT_MAX;

	for (size_t i = 0; i < targets_sz; i++) {
		// Copy pool of targets without the one we extract as head.
		struct Target * const new_targets = calloc(targets_sz-1,
				sizeof(struct Target));
		if (!new_targets) {
			printf("%s\n", strerror(errno));
			return -1;
		}

		size_t j = 0;
		for (size_t k = 0; k < targets_sz; k++) {
			if (k == i) {
				continue;
			}
			new_targets[j] = targets[k];
			j++;
		}

		current_targets[current_targets_sz] = targets[i];

		const int permutation_steps = __permute_targets(locations, new_targets,
				targets_sz-1, startx, starty, current_targets, current_targets_sz+1);
		if (permutation_steps == -1) {
			printf("__permute_targets\n");
			free(new_targets);
			return -1;
		}
		free(new_targets);

		if (permutation_steps < steps) {
			steps = permutation_steps;
		}
	}

	return steps;
}

// Visit all targets in the given order and find the shortest path to do this.
static int
__tour(enum Location * * const locations, struct Target * const targets,
		const size_t targets_sz, const int startx, const int starty)
{
	int steps = 0;

	int x = startx;
	int y = starty;

	for (size_t i = 0; i < targets_sz; i++) {
		const int path_steps = __shortest_path(locations, x, y, targets[i].x,
				targets[i].y);
		if (path_steps == -1) {
			printf("__shortest_path\n");
			return -1;
		}

		steps += path_steps;
		x = targets[i].x;
		y = targets[i].y;
	}

	// For part 2, we want to always return to our starting point.
	const int path_steps = __shortest_path(locations, x, y, startx, starty);
	if (path_steps == -1) {
		printf("__shortest_path\n");
		return -1;
	}

	steps += path_steps;

	return steps;
}

static int
__shortest_path(enum Location * * const locations, const int srcx,
		const int srcy, const int dstx, const int dsty)
{
#ifdef DEBUG
	printf("looking for path from %d,%d to %d,%d\n", srcx, srcy, dstx, dsty);
#endif

	struct Queue * const q = calloc(1, sizeof(struct Queue));
	if (!q) {
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (!__enqueue(q, srcx, srcy, 0)) {
		printf("__enqueue\n");
		destroy_queue(q, free);
		return -1;
	}

	struct htable * const visited = hash_init(HASH_SIZE);
	if (!visited) {
		printf("hash_init\n");
		destroy_queue(q, free);
		return -1;
	}

	while (q->first) {
		struct Position * const p = dequeue(q);
		if (!p) {
			printf("dequeue\n");
			destroy_queue(q, free);
			hash_free(visited, NULL);
			return -1;
		}

		if (p->x == dstx && p->y == dsty) {
			const int steps = p->steps;
			destroy_queue(q, free);
			hash_free(visited, NULL);
			free(p);
			return steps;
		}

		char * const p_str = calloc(64, sizeof(char));
		if (!p_str) {
			printf("%s\n", strerror(errno));
			destroy_queue(q, free);
			hash_free(visited, NULL);
			free(p);
			return -1;
		}
		snprintf(p_str, 64, "%d,%d", p->x, p->y);

		if (hash_has_key(visited, p_str)) {
			free(p);
			free(p_str);
			continue;
		}

		if (!hash_set(visited, p_str, NULL)) {
			printf("hash_set\n");
			destroy_queue(q, free);
			hash_free(visited, NULL);
			free(p);
			free(p_str);
			return -1;
		}

		if (!__add_neighbours(locations, q, p, p->steps)) {
			printf("__add_neighbours\n");
			destroy_queue(q, free);
			hash_free(visited, NULL);
			free(p);
			free(p_str);
			return -1;
		}

		free(p);
		free(p_str);
	}

	printf("no path found from %d,%d to %d,%d\n", srcx, srcy, dstx, dsty);
	destroy_queue(q, free);
	hash_free(visited, NULL);
	return -1;
}

static bool
__enqueue(struct Queue * const q, const int x, const int y, const int steps)
{
	struct Position * const p = calloc(1, sizeof(struct Position));
	if (!p) {
		printf("%s\n", strerror(errno));
		return false;
	}

	p->x = x;
	p->y = y;
	p->steps = steps;

	if (!enqueue(q, p)) {
		printf("enqueue\n");
		free(p);
		return false;
	}

	return true;
}

static bool
__add_neighbours(enum Location * * const locations, struct Queue * const q,
		const struct Position * const p, const int steps)
{
	const int x = p->x;
	const int y = p->y;

	// Up
	if (y > 0) {
		if (locations[x][y-1] == OPEN || locations[x][y-1] == TARGET) {
			if (!__enqueue(q, x, y-1, steps+1)) {
				printf("__enqueue\n");
				return false;
			}
		}
	}

	// Down
	if (y < MAX_HEIGHT) {
		if (locations[x][y+1] == OPEN || locations[x][y+1] == TARGET) {
			if (!__enqueue(q, x, y+1, steps+1)) {
				printf("__enqueue\n");
				return false;
			}
		}
	}

	// Right
	if (x < MAX_WIDTH) {
		if (locations[x+1][y] == OPEN || locations[x+1][y] == TARGET) {
			if (!__enqueue(q, x+1, y, steps+1)) {
				printf("__enqueue\n");
				return false;
			}
		}
	}

	// Left
	if (x > 0) {
		if (locations[x-1][y] == OPEN || locations[x-1][y] == TARGET) {
			if (!__enqueue(q, x-1, y, steps+1)) {
				printf("__enqueue\n");
				return false;
			}
		}
	}

	return true;
}
