#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int __count_safe_tiles(const char const *, const int);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input file> <rows>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];
	const int rows = atoi(argv[2]);

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	char * row = NULL;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		row = calloc(strlen(buf) + 1, sizeof(char));
		if (!row) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			return 1;
		}

		size_t j = 0;
		for (size_t i = 0; i < strlen(buf); i++) {
			if (!isspace(buf[i])) {
				row[j] = buf[i];
				j++;
			}
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		free(row);
		return 1;
	}

	const int safe_tiles = __count_safe_tiles(row, rows);
	printf("%d\n", safe_tiles);

	free(row);
	return 0;
}

static int
__count_safe_tiles(const char const * row, const int num_rows)
{
	int num_safe = 0;

	char * current_row = calloc(strlen(row) + 1, sizeof(char));
	if (!current_row) {
		printf("%s\n", strerror(errno));
		return -1;
	}
	memcpy(current_row, row, strlen(row) + 1);

	for (size_t i = 0; i < strlen(current_row); i++) {
		if (current_row[i] == '.') {
			num_safe++;
		}
	}

	// printf("%s\n", current_row);

	// num_rows-1 because we count our first row.
	for (int i = 0; i < num_rows - 1; i++) {
		char * const new_row = calloc(strlen(current_row) + 1, sizeof(char));
		if (!new_row) {
			printf("%s\n", strerror(errno));
			free(current_row);
			return -1;
		}

		for (size_t j = 0; j < strlen(current_row); j++) {
			char left = '.';
			char centre = current_row[j];
			char right = '.';

			if (j > 0) {
				left = current_row[j - 1];
			}
			if (j < strlen(current_row) - 1) {
				right = current_row[j + 1];
			}

			char next = '.';

			if (left == '^' && centre == '^' && right == '.') {
				next = '^';
			}
			if (left == '.' && centre == '^' && right == '^') {
				next = '^';
			}
			if (left == '^' && centre == '.' && right == '.') {
				next = '^';
			}
			if (left == '.' && centre == '.' && right == '^') {
				next = '^';
			}

			new_row[j] = next;

			if (next == '.') {
				num_safe++;
			}
		}

		memcpy(current_row, new_row, strlen(new_row) + 1);
		free(new_row);

		// printf("%s\n", current_row);
	}

	free(current_row);
	return num_safe;
}
