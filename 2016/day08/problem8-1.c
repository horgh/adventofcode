#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
__destroy_screen(bool * *, const size_t);
static void
__draw_rect(bool * const * const, const size_t,
		const size_t, const int, const int);
static void
__rotate_row(bool * const * const, const size_t,
		const size_t, const int, const int);
static void
__rotate_column(bool * const * const, const size_t,
		const size_t, const int, const int);
static void
__print_rect(bool * const * const, const size_t,
		const size_t);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input file> <print or not>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];
	bool const should_print = strcmp(argv[2], "1") == 0;

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	size_t width = 50;
	size_t height = 6;
	bool * * screen = calloc(width, sizeof(bool *));
	if (!screen) {
		printf("%s\n", strerror(ENOMEM));
		fclose(fh);
		return 1;
	}

	for (size_t i = 0; i < width; i++) {
		screen[i] = calloc(height, sizeof(bool));
		if (!screen[i]) {
			printf("%s\n", strerror(ENOMEM));
			fclose(fh);
			__destroy_screen(screen, width);
			return 1;
		}

		for (size_t j = 0; j < height; j++) {
			screen[i][j] = false;
		}
	}

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		int rect_width = 0;
		int rect_height = 0;
		int matches = sscanf(buf, "rect %dx%d", &rect_width, &rect_height);
		if (matches == 2) {
			__draw_rect(screen, width, height, rect_width, rect_height);
			if (false) {
				printf("After applying %s:\n", buf);
				__print_rect(screen, width, height);
			}
			continue;
		}

		int row_y = 0;
		int amount_y = 0;
		matches = sscanf(buf, "rotate row y=%d by %d", &row_y, &amount_y);
		if (matches == 2) {
			__rotate_row(screen, width, height, row_y, amount_y);
			if (false) {
				printf("After applying %s:\n", buf);
				__print_rect(screen, width, height);
			}
			continue;
		}

		int col_x = 0;
		int amount_x = 0;
		matches = sscanf(buf, "rotate column x=%d by %d", &col_x, &amount_x);
		if (matches == 2) {
			__rotate_column(screen, width, height, col_x, amount_x);
			if (false) {
				printf("After applying %s:\n", buf);
				__print_rect(screen, width, height);
			}
			continue;
		}

		printf("unrecognized input: %s\n", buf);
		fclose(fh);
		__destroy_screen(screen, width);
		return 1;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_screen(screen, width);
		return 1;
	}

	int count = 0;
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			if (screen[x][y]) {
				count++;
			}
		}
	}

	if (should_print) {
		__print_rect(screen, width, height);
	} else {
		printf("%d\n", count);
	}

	__destroy_screen(screen, width);

	return 0;
}

static void
__destroy_screen(bool * * screen, const size_t width)
{
	if (!screen) {
		return;
	}

	for (size_t x = 0; x < width; x++) {
		free(screen[x]);
	}

	free(screen);
}

static void
__draw_rect(bool * const * const screen, const size_t width,
		const size_t height, const int rect_width, const int rect_height)
{
	if (!screen) {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	if ((size_t) rect_width > width) {
		printf("rect width too large\n");
		return;
	}

	if ((size_t) rect_height > height) {
		printf("rect height too large\n");
		return;
	}

	for (size_t y = 0; y < (size_t) rect_height; y++) {
		for (size_t x = 0; x < (size_t) rect_width; x++) {
			screen[x][y] = true;
		}
	}
}

static void
__rotate_row(bool * const * const screen, const size_t width,
		const size_t height, const int y, const int amount)
{
	if (!screen) {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	if ((size_t) y > height) {
		printf("invalid y\n");
		return;
	}

	for (int i = 0; i < amount; i++) {
		bool prev = screen[width-1][y];

		for (size_t x = 0; x < width; x++) {
			bool next_prev = screen[x][y];
			screen[x][y] = prev;
			prev = next_prev;
		}
	}
}

static void
__rotate_column(bool * const * const screen, const size_t width,
		const size_t height, const int x, const int amount)
{
	if (!screen) {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	if ((size_t ) x > width) {
		printf("invalid x\n");
		return;
	}

	for (int i = 0; i < amount; i++) {
		bool prev = screen[x][height-1];

		for (size_t y = 0; y < height; y++) {
			bool next_prev = screen[x][y];
			screen[x][y] = prev;
			prev = next_prev;
		}
	}
}

static void
__print_rect(bool * const * const screen, const size_t width,
		const size_t height)
{
	if (!screen) {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			if (screen[x][y]) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}
