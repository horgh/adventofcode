#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOWN 0
#define UP 1
#define RIGHT 2
#define LEFT 3

#define DEBUG 0

static void destroy_map(char ** const, size_t const);
static bool travel(char ** const,
		size_t const,
		size_t const,
		size_t const,
		size_t const,
		char * const,
		size_t * const,
		int const);
static void record_letter(
		char ** const, size_t const, size_t const, char * const, size_t * const);
static bool can_move(char ** const,
		size_t const,
		size_t const,
		size_t const,
		size_t const,
		int const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;
	char buf[4096] = {0};

	char * map[256] = {0};
	for (size_t i = 0; i < 256; i++) {
		map[i] = calloc(256, sizeof(char));
		if (!map[i]) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			return 1;
		}
	}

	size_t col_max = 0;
	size_t row_max = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				destroy_map(map, 256);
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

		strcpy(map[row_max], buf);

		if (col_max == 0) {
			col_max = strlen(buf);
		} else {
			if (strlen(buf) != col_max) {
				fprintf(stderr, "line is different length\n");
				destroy_map(map, 256);
				return 1;
			}
		}

		row_max++;
	}

	size_t row = 0;
	size_t col = 0;
	for (size_t i = 0; i < col_max; i++) {
		if (map[row][i] == '|') {
			col = i;
			break;
		}
	}

	char visited[32] = {0};
	size_t visited_size = 0;

	travel(map, row_max, col_max, row, col, visited, &visited_size, DOWN);
	destroy_map(map, 256);
	printf("\n");

	return 0;
}

static void
destroy_map(char ** const map, size_t const sz)
{
	if (!map) {
		return;
	}

	for (size_t i = 0; i < sz; i++) {
		free(map[i]);
	}
}

static bool
travel(char ** const map,
		size_t const row_max,
		size_t const col_max,
		size_t const row,
		size_t const col,
		char * const visited,
		size_t * const visited_size,
		int const direction)
{
	record_letter(map, row, col, visited, visited_size);

	switch (direction) {
	case DOWN:
		if (can_move(map, row_max, col_max, row, col, DOWN)) {
			return travel(
					map, row_max, col_max, row + 1, col, visited, visited_size, DOWN);
		}
		if (can_move(map, row_max, col_max, row, col, RIGHT)) {
			if (travel(map,
							row_max,
							col_max,
							row,
							col + 1,
							visited,
							visited_size,
							RIGHT)) {
				return true;
			}
		}
		if (can_move(map, row_max, col_max, row, col, LEFT)) {
			if (travel(map,
							row_max,
							col_max,
							row,
							col - 1,
							visited,
							visited_size,
							LEFT)) {
				return true;
			}
		}
		if (DEBUG) {
			fprintf(stderr, "stuck going down\n");
		}
		return true;
		break;
	case UP:
		if (can_move(map, row_max, col_max, row, col, UP)) {
			return travel(
					map, row_max, col_max, row - 1, col, visited, visited_size, UP);
		}
		if (can_move(map, row_max, col_max, row, col, RIGHT)) {
			if (travel(map,
							row_max,
							col_max,
							row,
							col + 1,
							visited,
							visited_size,
							RIGHT)) {
				return true;
			}
		}
		if (can_move(map, row_max, col_max, row, col, LEFT)) {
			if (travel(map,
							row_max,
							col_max,
							row,
							col - 1,
							visited,
							visited_size,
							LEFT)) {
				return true;
			}
		}
		if (DEBUG) {
			fprintf(stderr, "stuck going up\n");
		}
		return true;
		break;
	case RIGHT:
		if (can_move(map, row_max, col_max, row, col, RIGHT)) {
			return travel(
					map, row_max, col_max, row, col + 1, visited, visited_size, RIGHT);
		}
		if (can_move(map, row_max, col_max, row, col, UP)) {
			if (travel(
							map, row_max, col_max, row - 1, col, visited, visited_size, UP)) {
				return true;
			}
		}
		if (can_move(map, row_max, col_max, row, col, DOWN)) {
			if (travel(map,
							row_max,
							col_max,
							row + 1,
							col,
							visited,
							visited_size,
							DOWN)) {
				return true;
			}
		}
		if (DEBUG) {
			fprintf(stderr, "stuck going right\n");
		}
		return true;
		break;
	case LEFT:
		if (can_move(map, row_max, col_max, row, col, LEFT)) {
			return travel(
					map, row_max, col_max, row, col - 1, visited, visited_size, LEFT);
		}
		if (can_move(map, row_max, col_max, row, col, UP)) {
			if (travel(
							map, row_max, col_max, row - 1, col, visited, visited_size, UP)) {
				return true;
			}
		}
		if (can_move(map, row_max, col_max, row, col, DOWN)) {
			if (travel(map,
							row_max,
							col_max,
							row + 1,
							col,
							visited,
							visited_size,
							DOWN)) {
				return true;
			}
		}
		if (DEBUG) {
			fprintf(stderr, "stuck going left\n");
		}
		return true;
		break;
	default:
		fprintf(stderr, "%s: unknown direction\n", __func__);
		return false;
		break;
	}
}

static void
record_letter(char ** const map,
		size_t const row,
		size_t const col,
		char * const visited,
		size_t * const visited_size)
{
	if (map[row][col] < 'A' || map[row][col] > 'Z') {
		return;
	}

	for (size_t i = 0; i < *visited_size; i++) {
		if (visited[i] == map[row][col]) {
			return;
		}
	}

	printf("%c", map[row][col]);

	visited[*visited_size] = map[row][col];
	*visited_size += 1;
}

static bool
can_move(char ** const map,
		size_t const row_max,
		size_t const col_max,
		size_t const row,
		size_t const col,
		int const direction)
{
	switch (direction) {
	case DOWN:
		if (row == row_max - 1) {
			return false;
		}
		return map[row + 1][col] != ' ';
		break;
	case UP:
		if (row == 0) {
			return false;
		}
		return map[row - 1][col] != ' ';
		break;
	case RIGHT:
		if (col == col_max - 1) {
			return false;
		}
		return map[row][col + 1] != ' ';
		break;
	case LEFT:
		if (col == 0) {
			return false;
		}
		return map[row][col - 1] != ' ';
		break;
	default:
		fprintf(stderr, "%s: invalid direction\n", __func__);
		return false;
	}
}
