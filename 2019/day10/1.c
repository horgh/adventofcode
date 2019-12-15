#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Point {
	bool asteroid;
};

static int count(struct Point * * const,
		int const,
		int const,
		int const,
		int const);
static bool double_equal(double const, double const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Point * * const map = calloc(32, sizeof(struct Point *));
	assert(map != NULL);
	for (size_t i = 0; i < 32; i++) {
		map[i] = calloc(32, sizeof(struct Point));
		assert(map[i] != NULL);
	}

	int x = 0, y = 0;
	int max_x = -1, max_y = -1;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		char const * ptr = buf;
		while (*ptr != '\n') {
			if (*ptr == '#') {
				map[x][y].asteroid = true;
			}
			x++;
			if (max_x == -1 || x > max_x) {
				max_x = x;
			}
			ptr++;
		}
		x = 0;
		y++;
	}
	max_y = y;

	int max = -1;
	int x2 = -1, y2 = -1;
	for (y = 0; y < max_y; y++) {
		for (x = 0; x < max_x; x++) {
			if (!map[x][y].asteroid) {
				continue;
			}
			int const c = count(map, max_x, max_y, x, y);
			if (max == -1 || c > max) {
				max = c;
				x2 = x;
				y2 = y;
			}
		}
	}

	printf("%d at %d,%d\n", max, x2, y2);
	return 0;
}

static int count(struct Point * * const map,
		int const max_x,
		int const max_y,
		int const x,
		int const y) {
	double slopes_right[512] = {0};
	double slopes_left[512] = {0};
	size_t n_slopes_left = 0, n_slopes_right = 0;
	int verticals[2] = {0};
	int horizontals[2] = {0};
	for (int y2 = 0; y2 < max_y; y2++) {
		for (int x2 = 0; x2 < max_x; x2++) {
			if (x2 == x && y2 == y) {
				continue;
			}
			if (!map[x2][y2].asteroid) {
				continue;
			}

			int const denom = x2-x;
			if (denom == 0) {
				if (y2 < y) { // Vertical up
					verticals[0] = 1;
				}
				if (y2 > y) { // Vertical down
					verticals[1] = 1;
				}
				continue;
			}
			int const numerator = y2-y;
			if (numerator == 0) {
				if (x2 < x) { // Horizontal, left
					horizontals[0] = 1;
				}
				if (x2 > x) { // Horizontal, right
					horizontals[1] = 1;
				}
				continue;
			}
			double const slope = (double) numerator/(double) denom;

			if (x2 > x) {
				bool have_slope = false;
				for (size_t i = 0; i < n_slopes_right; i++) {
					if (double_equal(slopes_right[i], slope)) {
						have_slope = true;
						break;
					}
				}
				if (!have_slope) {
					slopes_right[n_slopes_right++] = slope;
				}
			} else {
				bool have_slope = false;
				for (size_t i = 0; i < n_slopes_left; i++) {
					if (double_equal(slopes_left[i], slope)) {
						have_slope = true;
						break;
					}
				}
				if (!have_slope) {
					slopes_left[n_slopes_left++] = slope;
				}
			}
		}
	}

	return (int) n_slopes_right + (int) n_slopes_left +
		verticals[0] + verticals[1] +
		horizontals[0] + horizontals[1];
}

__attribute__((const))
static bool double_equal(double const a, double const b) {
	return fabs(a-b) < 0.0001;
}
