#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 4096

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define DEBUG 0

static void
destroy_grid(bool * * const, size_t const);
static void
print_grid(
		bool * * const,
		size_t const,
		size_t const,
		size_t const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <steps>\n", argv[0]);
		return 1;
	}
	int const steps = atoi(argv[1]);

	FILE * const fh = stdin;
	char buf[4096] = {0};

	bool * * const grid = calloc(MAP_SIZE, sizeof(bool *));
	if (!grid) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}
	for (size_t i = 0; i < MAP_SIZE; i++) {
		grid[i] = calloc(MAP_SIZE, sizeof(bool));
		if (!grid[i]) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			destroy_grid(grid, i);
			return 1;
		}
	}

	size_t const row_start = MAP_SIZE/2;
	size_t const col_start = MAP_SIZE/2;

	if (DEBUG) {
		printf("map starts at %zu, %zu\n", row_start, col_start);
	}

	size_t num_rows = 0;

	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				destroy_grid(grid, MAP_SIZE);
				return 1;
			}
			break;
		}

		char * ptr = buf;
		while (*ptr != '\0') {
			if (*ptr == '\n') {
				*ptr = '\0';
				break;
			}
			ptr++;
		}

		ptr = buf;
		size_t col = col_start;
		while (*ptr != '\0') {
			if (*ptr == '#') {
				grid[row_start+num_rows][col] = true;
			}
			col++;
			ptr++;
		}

		num_rows++;
	}

	size_t const initial_dimensions = num_rows;

	size_t pos_row = row_start+(initial_dimensions/2);
	size_t pos_col = col_start+(initial_dimensions/2);

	if (DEBUG) {
		printf("starting at %zu, %zu\n", pos_row, pos_col);

		print_grid(grid, row_start, col_start, num_rows);
	}

	int direction = UP;
	int infections = 0;

	for (int i = 0; i < steps; i++) {
		bool const infected = grid[pos_row][pos_col];

		if (infected) {
			switch (direction) {
			case UP:
				direction = RIGHT;
				break;
			case RIGHT:
				direction = DOWN;
				break;
			case DOWN:
				direction = LEFT;
				break;
			case LEFT:
				direction = UP;
				break;
			default:
				fprintf(stderr, "unexpected direction\n");
				destroy_grid(grid, MAP_SIZE);
				return 1;
			}
			grid[pos_row][pos_col] = false;
		} else {
			switch (direction) {
			case UP:
				direction = LEFT;
				break;
			case RIGHT:
				direction = UP;
				break;
			case DOWN:
				direction = RIGHT;
				break;
			case LEFT:
				direction = DOWN;
				break;
			default:
				fprintf(stderr, "unexpected direction\n");
				destroy_grid(grid, MAP_SIZE);
				return 1;
			}
			grid[pos_row][pos_col] = true;
			infections++;
		}

		switch (direction) {
		case UP:
			pos_row--;
			break;
		case RIGHT:
			pos_col++;
			break;
		case DOWN:
			pos_row++;
			break;
		case LEFT:
			pos_col--;
			break;
		default:
			fprintf(stderr, "unexpected direction\n");
			destroy_grid(grid, MAP_SIZE);
			return 1;
		}
	}

	destroy_grid(grid, MAP_SIZE);
	printf("%d\n", infections);
	return 0;
}

static void
destroy_grid(bool * * const g, size_t const sz)
{
	if (!g) {
		return;
	}

	for (size_t i = 0; i < sz; i++) {
		free(g[i]);
	}

	free(g);
}

static void
print_grid(
		bool * * const g,
		size_t const row,
		size_t const col,
		size_t const sz)
{
	for (size_t i = 0; i < sz; i++) {
		for (size_t j = 0; j < sz; j++) {
			if (g[row+i][col+j]) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}
