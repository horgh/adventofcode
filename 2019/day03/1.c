#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fill(
		int * * const,
		size_t const,
		size_t const,
		int const,
		char const * const);
static void mark_wire(
		int * * const,
		size_t const,
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

	int * * const grid = calloc(grid_size, sizeof(int *));
	assert(grid != NULL);
	for (size_t i = 0; i < grid_size; i++) {
		grid[i] = calloc(grid_size, sizeof(int));
		assert(grid[i] != NULL);
	}

	int n = 1;
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
			if (grid[x][y] == -1) {
				int const distance = abs(x-(int) centre) + abs(y-(int) centre);
				if (smallest == -1 || distance < smallest) {
					smallest = distance;
				}
			}
		}
	}

	printf("%d\n", smallest);

	return 0;
}

static void fill(
		int * * const grid,
		size_t const grid_size,
		size_t const centre,
		int const n,
		char const * const buf) {
	int x = (int) centre, y = (int) centre;
	char const * ptr = buf;
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
				for (; y < new_y; y++) {
					mark_wire(grid, grid_size, n, x, y);
				}
				break;
			case 'R':
				new_x = x+amount;
				for (; x < new_x; x++) {
					mark_wire(grid, grid_size, n, x, y);
				}
				break;
			case 'D':
				new_y = y-amount;
				for (; y > new_y; y--) {
					mark_wire(grid, grid_size, n, x, y);
				}
				break;
			case 'L':
				new_x = x-amount;
				for (; x > new_x; x--) {
					mark_wire(grid, grid_size, n, x, y);
				}
				break;
			default:
				printf("unexpected direction %c\n", direction);
				abort();
		}
	}
}

static void mark_wire(
		int * * const grid,
		size_t const grid_size,
		int const n,
		int const x,
		int const y) {
	if (x < 0 || y < 0 || x >= (int) grid_size || y >= (int) grid_size) {
		printf("invalid position %d, %d\n", x, y);
		abort();
	}
	if (grid[x][y] == 0) {
		grid[x][y] = n;
		return;
	}
	if (grid[x][y] > 0 && grid[x][y] != n) {
		grid[x][y] = -1; // Cross
	}
}
