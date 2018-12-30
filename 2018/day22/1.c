#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Type { Rocky, Narrow, Wet };

struct Position {
	enum Type type;
	int erosion_level;
};

static void draw(struct Position ** const, int const, int const, int const);

static void get_type(struct Position ** const,
		int const,
		int const,
		int const,
		int const,
		int const);
static int get_geologic_index(
		struct Position ** const, int const, int const, int const, int const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char buf[4096] = {0};

	assert(fgets(buf, 4096, stdin) != NULL);
	trim_right(buf);
	char const * ptr = buf;
	ptr += strlen("depth: ");
	int const depth = atoi(ptr);
	memset(buf, 0, 4096);

	assert(fgets(buf, 4096, stdin) != NULL);
	trim_right(buf);
	ptr = buf;
	ptr += strlen("target: ");
	int const target_x = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	ptr++;
	int const target_y = atoi(ptr);

	if (0) {
		printf("depth: %d target: %d,%d\n", depth, target_x, target_y);
	}

	size_t sz = (size_t)(target_x + 1);
	if ((size_t)(target_y + 1) > sz) {
		sz = (size_t)(target_y + 1);
	}

	struct Position ** const map = calloc(sz, sizeof(struct Position *));
	assert(map != NULL);
	for (size_t i = 0; i < sz; i++) {
		map[i] = calloc(sz, sizeof(struct Position));
		assert(map[i] != NULL);
	}

	draw(map, depth, target_x, target_y);

	if (0) {
		for (int y = 0; y <= target_y; y++) {
			for (int x = 0; x <= target_x; x++) {
				if (x == 0 && y == 0) {
					printf("M");
					continue;
				}
				if (x == target_x && y == target_y) {
					printf("T");
					continue;
				}
				if (map[x][y].type == Rocky) {
					printf(".");
					continue;
				}
				if (map[x][y].type == Wet) {
					printf("=");
					continue;
				}
				if (map[x][y].type == Narrow) {
					printf("|");
					continue;
				}
				assert(1 == 0);
			}
			printf("\n");
		}
	}

	int risk = 0;
	for (int y = 0; y <= target_y; y++) {
		for (int x = 0; x <= target_x; x++) {
			if (map[x][y].type == Wet) {
				risk++;
				continue;
			}
			if (map[x][y].type == Narrow) {
				risk += 2;
				continue;
			}
		}
	}
	printf("%d\n", risk);
	return 0;
}

static void
draw(struct Position ** const map,
		int const depth,
		int const target_x,
		int const target_y)
{
	for (int y = 0; y <= target_y; y++) {
		for (int x = 0; x <= target_x; x++) {
			get_type(map, depth, target_x, target_y, x, y);
		}
	}
}

static void
get_type(struct Position ** const map,
		int const depth,
		int const target_x,
		int const target_y,
		int const x,
		int const y)
{
	int const geologic_index = get_geologic_index(map, target_x, target_y, x, y);
	int const erosion_level = (geologic_index + depth) % 20183;
	map[x][y].erosion_level = erosion_level;
	int const m = erosion_level % 3;
	if (m == 0) {
		map[x][y].type = Rocky;
		return;
	}
	if (m == 1) {
		map[x][y].type = Wet;
		return;
	}
	map[x][y].type = Narrow;
}

__attribute__((pure)) static int
get_geologic_index(struct Position ** const map,
		int const target_x,
		int const target_y,
		int const x,
		int const y)
{
	if (x == 0 && y == 0) {
		return 0;
	}
	if (x == target_x && y == target_y) {
		return 0;
	}
	if (y == 0) {
		return x * 16807;
	}
	if (x == 0) {
		return y * 48271;
	}
	assert(x - 1 >= 0);
	assert(y - 1 >= 0);
	return map[x - 1][y].erosion_level * map[x][y - 1].erosion_level;
}
