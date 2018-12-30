#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#define SZ 2048

enum GroundType { Sand, Clay, Spring, HadWater, StillWater };

struct Position {
	enum GroundType type;
};

static void print_map(struct Position ** const);

static void print_position(struct Position const * const);

static int move(struct Position ** const, int const, int const, int const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Position ** const map = calloc(SZ, sizeof(struct Position *));
	assert(map != NULL);
	for (size_t i = 0; i < SZ; i++) {
		map[i] = calloc(SZ, sizeof(struct Position));
		assert(map[i] != NULL);
	}
	for (int y = 0; y < SZ; y++) {
		for (int x = 0; x < SZ; x++) {
			map[x][y].type = Sand;
		}
	}
	map[500][0].type = Spring;
	int min_y = -1;
	int max_y = -1;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		char const x_or_y = *ptr;

		ptr += 2;
		int const n = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += strlen(", y=");
		int const m_start = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += 2;
		int const m_end = atoi(ptr);

		if (x_or_y == 'x') {
			for (int i = m_start; i <= m_end; i++) {
				map[n][i].type = Clay;
				assert(i < SZ && n < SZ);
				if (min_y == -1) {
					min_y = i;
				}
				if (max_y == -1) {
					max_y = i;
				}
				if (i > max_y) {
					max_y = i;
				}
				if (i < min_y) {
					min_y = i;
				}
			}
			continue;
		}
		if (x_or_y == 'y') {
			if (min_y == -1) {
				min_y = n;
			}
			if (max_y == -1) {
				max_y = n;
			}
			if (n > max_y) {
				max_y = n;
			}
			if (n < min_y) {
				min_y = n;
			}
			for (int i = m_start; i <= m_end; i++) {
				map[i][n].type = Clay;
				assert(i < SZ && n < SZ);
			}
			continue;
		}
		assert(1 == 0);
	}

	if (0) {
		print_map(map);
	}

	int x = 500;
	int y = 0;
	move(map, x, y + 1, max_y);

	int count_p1 = 0;
	int count_p2 = 0;
	for (y = min_y; y <= max_y; y++) {
		for (x = 0; x < SZ; x++) {
			if (map[x][y].type == HadWater || map[x][y].type == StillWater) {
				count_p1++;
			}
			if (map[x][y].type == StillWater) {
				count_p2++;
			}
		}
	}

	for (size_t i = 0; i < SZ; i++) {
		free(map[i]);
	}
	free(map);
	printf("%d\n", count_p1);
	printf("%d\n", count_p2);
	return 0;
}

static void
print_map(struct Position ** const map)
{
	int x0 = 0;
	int x1 = 586;
	int y0 = 0;
	int y1 = 1758;
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			print_position(&map[x][y]);
		}
		printf("\n");
	}
}

static void
print_position(struct Position const * const p)
{
	if (p->type == Sand) {
		printf(".");
		return;
	}
	if (p->type == Clay) {
		printf("#");
		return;
	}
	if (p->type == Spring) {
		printf("+");
		return;
	}
	if (p->type == HadWater) {
		printf("|");
		return;
	}
	if (p->type == StillWater) {
		printf("~");
		return;
	}
	assert(1 == 0);
}

static int
move(struct Position ** const map, int const x, int const y, int const max_y)
{
	if (y == SZ || x < 0 || x == SZ) {
		return 0;
	}

	if (map[x][y].type == HadWater || map[x][y].type == StillWater ||
			map[x][y].type == Clay) {
		return 0;
	}

	if (map[x][y].type == Sand) {
		map[x][y].type = HadWater;
		if (y == max_y) {
			return 1;
		}
		if (map[x][y + 1].type == HadWater) {
			return 1;
		}
		if (move(map, x, y + 1, max_y)) {
			return 1;
		}
	}

	int const left = move(map, x - 1, y, max_y);
	int const right = move(map, x + 1, y, max_y);

	if (left || right) {
		for (int x2 = x; x2 >= 0; x2--) {
			if (map[x2][y].type != StillWater && map[x2][y].type != HadWater) {
				break;
			}
			map[x2][y].type = HadWater;
		}
		for (int x2 = x; x2 < SZ; x2++) {
			if (map[x2][y].type != StillWater && map[x2][y].type != HadWater) {
				break;
			}
			map[x2][y].type = HadWater;
		}
		return 1;
	}

	for (int x2 = x; x2 >= 0; x2--) {
		if (map[x2][y].type != HadWater) {
			break;
		}
		map[x2][y].type = StillWater;
	}
	for (int x2 = x; x2 < SZ; x2++) {
		if (map[x2][y].type != HadWater) {
			break;
		}
		map[x2][y].type = StillWater;
	}
	return 0;
}
