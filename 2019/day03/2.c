#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct position {
	int wire_steps[2];
};

static void fill(
		struct position * * const,
		size_t const,
		size_t const,
		int const,
		char const * const);
static void mark_wire(
		struct position * * const,
		size_t const,
		int const,
		int const,
		int const,
		int const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <size>\n", argv[0]);
		return 1;
	}
	size_t const grid_size = (size_t) atoi(argv[1]);
	size_t const centre = grid_size / 2;

	struct position * * const grid = calloc(grid_size,
			sizeof(struct position *));
	assert(grid != NULL);
	for (size_t i = 0; i < grid_size; i++) {
		grid[i] = calloc(grid_size, sizeof(struct position));
		assert(grid[i] != NULL);
	}

	int n = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		fill(grid, grid_size, centre, n, buf);
		n++;
	}

	int smallest = -1;
	for (int x = 0; x < (int) grid_size; x++) {
		for (int y = 0; y < (int) grid_size; y++) {
			if (x == (int) centre && y == (int) centre ) {
				continue;
			}
			if (grid[x][y].wire_steps[0] == 0 ||
					grid[x][y].wire_steps[1] == 0) {
				continue;
			}

			int const steps = grid[x][y].wire_steps[0] +
					grid[x][y].wire_steps[1];
			if (smallest == -1 || steps < smallest) {
				smallest = steps;
			}
		}
	}

	printf("%d\n", smallest);

	return 0;
}

static void fill(
		struct position * * const grid,
		size_t const grid_size,
		size_t const centre,
		int const n,
		char const * const buf) {
	int x = (int) centre, y = (int) centre;
	char const * ptr = buf;
	int steps = 0;
	while (*ptr != '\0' && *ptr != '\n') {
		char const direction = *ptr;
		ptr++;
		int const amount = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		if (*ptr == ',') {
			ptr++;
		}
		int new_x = 0, new_y = 0;
		switch (direction) {
			case 'U':
				new_y = y+amount;
				while (y < new_y) {
					y++;
					steps++;
					mark_wire(grid, grid_size, n, x, y, steps);
				}
				break;
			case 'R':
				new_x = x+amount;
				while (x < new_x) {
					x++;
					steps++;
					mark_wire(grid, grid_size, n, x, y, steps);
				}
				break;
			case 'D':
				new_y = y-amount;
				while (y > new_y) {
					y--;
					steps++;
					mark_wire(grid, grid_size, n, x, y, steps);
				}
				break;
			case 'L':
				new_x = x-amount;
				while (x > new_x) {
					x--;
					steps++;
					mark_wire(grid, grid_size, n, x, y, steps);
				}
				break;
			default:
				printf("unexpected direction %c\n", direction);
				abort();
		}
	}
}

static void mark_wire(
		struct position * * const grid,
		size_t const grid_size,
		int const n,
		int const x,
		int const y,
		int const steps) {
	if (x < 0 || y < 0 || x >= (int) grid_size || y >= (int) grid_size) {
		printf("invalid position %d, %d\n", x, y);
		abort();
	}
	if (grid[x][y].wire_steps[n] == 0) {
		grid[x][y].wire_steps[n] = steps;
		return;
	}
}
