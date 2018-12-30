#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ** grid_new(int const);
static void grid_destroy(int ** const, int const);
static int grid_get(int ** const, int const, int const, int const);

int
main(const int argc, const char * const * const argv)
{
	int input = 0;

	if (argc >= 2) {
		input = atoi(argv[1]);
	} else {
		FILE * const fh = stdin;

		char buf[10240] = {0};

		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			fprintf(stderr, "fgets(): %s\n", strerror(errno));
			return 1;
		}

		input = atoi(buf);
	}

	for (int side_length = 1;; side_length++) {
		int const num_squares = side_length * side_length;
		if (num_squares % 2 == 0) {
			continue;
		}

		int ** const grid = grid_new(side_length);
		if (!grid) {
			return 1;
		}

		int const row_idx_centre = side_length / 2;
		int const col_idx_centre = side_length / 2;

		grid[row_idx_centre][col_idx_centre] = 1;

		for (int square = 2; square <= num_squares; square++) {
			int cur_side_length = 0;
			int cur_num_squares = 0;
			while (1) {
				cur_num_squares = cur_side_length * cur_side_length;
				if (cur_num_squares >= square && cur_num_squares % 2 != 0) {
					break;
				}

				cur_side_length++;
			}

			int const bottom_right = cur_num_squares;
			int const bottom_left = bottom_right - cur_side_length + 1;
			int const top_left = bottom_left - cur_side_length + 1;
			int const top_right = top_left - cur_side_length + 1;

			int row_idx = 0;
			int col_idx = 0;
			if (square >= bottom_left) {
				// Bottom side.
				int const mid = bottom_right - (cur_side_length / 2);
				int const steps_horizontal = abs(mid - square);
				int const steps_vertical = cur_side_length / 2;

				row_idx = row_idx_centre + steps_vertical;
				if (square > mid) {
					col_idx = col_idx_centre + steps_horizontal;
				} else {
					col_idx = col_idx_centre - steps_horizontal;
				}
			} else if (square >= top_left) {
				// Left side.
				int const mid = bottom_left - (cur_side_length / 2);
				int const steps_horizontal = cur_side_length / 2;
				int const steps_vertical = abs(mid - square);

				if (square > mid) {
					row_idx = row_idx_centre + steps_vertical;
				} else {
					row_idx = row_idx_centre - steps_vertical;
				}
				col_idx = col_idx_centre - steps_horizontal;
			} else if (square >= top_right) {
				// Top side.
				int const mid = top_left - (cur_side_length / 2);
				int const steps_horizontal = abs(mid - square);
				int const steps_vertical = cur_side_length / 2;

				row_idx = row_idx_centre - steps_vertical;
				if (square > mid) {
					col_idx = col_idx_centre - steps_horizontal;
				} else {
					col_idx = col_idx_centre + steps_horizontal;
				}
			} else {
				// Right side.
				int const mid = top_right - (cur_side_length / 2);
				int const steps_horizontal = cur_side_length / 2;
				int const steps_vertical = abs(mid - square);

				if (square > mid) {
					row_idx = row_idx_centre - steps_vertical;
				} else {
					row_idx = row_idx_centre + steps_vertical;
				}
				col_idx = col_idx_centre + steps_horizontal;
			}

			int const up_left = grid_get(grid, side_length, row_idx - 1, col_idx - 1);
			int const up = grid_get(grid, side_length, row_idx - 1, col_idx);
			int const up_right =
					grid_get(grid, side_length, row_idx - 1, col_idx + 1);
			int const right = grid_get(grid, side_length, row_idx, col_idx + 1);
			int const down_right =
					grid_get(grid, side_length, row_idx + 1, col_idx + 1);
			int const down = grid_get(grid, side_length, row_idx + 1, col_idx);
			int const down_left =
					grid_get(grid, side_length, row_idx + 1, col_idx - 1);
			int const left = grid_get(grid, side_length, row_idx, col_idx - 1);

			int const val = up_left + up + up_right + right + down_right + down +
											down_left + left;
			if (val > input) {
				printf("%d\n", val);
				grid_destroy(grid, side_length);
				return 0;
			}

			grid[row_idx][col_idx] = val;
		}

		grid_destroy(grid, side_length);
	}

	return 0;
}

static int **
grid_new(int const side_length)
{
	int ** const grid = calloc((size_t)side_length, sizeof(int *));
	if (!grid) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	for (int i = 0; i < side_length; i++) {
		int * const row = calloc((size_t)side_length, sizeof(int));
		if (!row) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			grid_destroy(grid, side_length);
			return NULL;
		}

		*(grid + i) = row;
	}

	return grid;
}

static void
grid_destroy(int ** const grid, int const side_length)
{
	if (!grid) {
		return;
	}

	for (int i = 0; i < side_length; i++) {
		int * const row = *(grid + i);
		if (!row) {
			break;
		}

		free(row);
	}

	free(grid);
}

static int
grid_get(int ** const grid,
		int const side_length,
		int const row_idx,
		int const col_idx)
{
	if (row_idx < 0 || col_idx < 0 || row_idx >= side_length ||
			col_idx >= side_length) {
		return 0;
	}
	return grid[row_idx][col_idx];
}
