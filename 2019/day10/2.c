#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <float.h>
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
	int direction;
	double slope;
	bool vaporized;
};

#define UP     1
#define DOWN   2
#define RIGHT  3
#define LEFT   4

#define UPRIGHT   5
#define DOWNRIGHT 6
#define DOWNLEFT  7
#define UPLEFT    8

static int count(struct Point * * const,
		int const,
		int const,
		int const,
		int const);
static bool double_equal(double const, double const);
static int attack(struct Point * * const,
		int const,
		int const,
		int const,
		int const);
static bool vaporize_up_right_down_left(struct Point * * const,
		int const,
		int const,
		int const,
		int const,
		int const);
static bool vaporize_decreasing(struct Point * * const,
		int const,
		int const,
		int const,
		int const,
		int const,
		double * const);
static bool vaporize_increasing(struct Point * * const,
		int const,
		int const,
		int const,
		int const,
		int const,
		double * const);
static int calc_distance(
	int const,
	int const,
	int const,
	int const);

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

	int const answer = attack(map, max_x, max_y, x2, y2);
	printf("%d\n", answer);
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

static int attack(struct Point * * const map,
		int const max_x,
		int const max_y,
		int const x,
		int const y) {
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
					map[x2][y2].direction = UP;
				}
				if (y2 > y) { // Vertical down
					map[x2][y2].direction = DOWN;
				}
				continue;
			}
			int const numerator = y2-y;
			if (numerator == 0) {
				if (x2 < x) { // Horizontal, left
					map[x2][y2].direction = LEFT;
				}
				if (x2 > x) { // Horizontal, right
					map[x2][y2].direction = RIGHT;
				}
				continue;
			}

			map[x2][y2].slope = fabs((double) numerator/(double) denom);

			if (x2 > x && y2 < y) {
				map[x2][y2].direction = UPRIGHT;
			}
			if (x2 > x && y2 > y) {
				map[x2][y2].direction = DOWNRIGHT;
			}
			if (x2 < x && y2 > y) {
				map[x2][y2].direction = DOWNLEFT;
			}
			if (x2 < x && y2 < y) {
				map[x2][y2].direction = UPLEFT;
			}
		}
	}

	int direction = UP;
	int count = 0;
	while (count < 200) {
		if (direction == UP || direction == RIGHT || direction == DOWN ||
				direction == LEFT) {
			if (vaporize_up_right_down_left(map, max_x, max_y, x, y, direction)) {
				count++;
				printf(" #%d\n", count);
			}
			switch (direction) {
			case UP:
				direction = UPRIGHT;
				break;
			case RIGHT:
				direction = DOWNRIGHT;
				break;
			case DOWN:
				direction = DOWNLEFT;
				break;
			case LEFT:
				direction = UPLEFT;
				break;
			default:
				printf("invalid direction\n");
				exit(1);
			}
			continue;
		}

		if (direction == UPRIGHT || direction == DOWNLEFT) {
			double last_slope = DBL_MAX;
			while (vaporize_decreasing(map, max_x, max_y, x, y, direction,
						&last_slope)) {
				count++;
				printf(" #%d\n", count);
			}
			switch (direction) {
			case UPRIGHT:
				direction = RIGHT;
				break;
			case DOWNLEFT:
				direction = LEFT;
				break;
			default:
				printf("invalid direction\n");
				exit(1);
			}
			continue;
		}

		if (direction == DOWNRIGHT || direction == UPLEFT) {
			double last_slope = DBL_MIN;
			while (vaporize_increasing(map, max_x, max_y, x, y, direction,
						&last_slope)) {
				count++;
				printf(" #%d\n", count);
			}
			switch (direction) {
			case DOWNRIGHT:
				direction = DOWN;
				break;
			case UPLEFT:
				direction = UP;
				break;
			default:
				printf("invalid direction\n");
				exit(1);
			}
			continue;
		}
	}

	return -1;
}

static bool vaporize_up_right_down_left(struct Point * * const map,
		int const max_x,
		int const max_y,
		int const x,
		int const y,
		int const direction) {
	int closest_distance = -1;
	int x3 = 0, y3 = 0;
	for (int y2 = 0; y2 < max_y; y2++) {
		for (int x2 = 0; x2 < max_x; x2++) {
			if (x2 == x && y2 == y) {
				continue;
			}
			if (!map[x2][y2].asteroid) {
				continue;
			}
			if (map[x2][y2].vaporized) {
				continue;
			}
			if (direction != map[x2][y2].direction) {
				continue;
			}

			int const distance = calc_distance(x, y, x2, y2);
			if (closest_distance == -1 || distance < closest_distance) {
				closest_distance = distance;
				x3 = x2;
				y3 = y2;
			}
		}
	}

	if (closest_distance == -1) {
		return false;
	}
	printf("vaporize %d,%d (answer: %d)", x3, y3, x3*100+y3);
	map[x3][y3].vaporized = true;
	return true;
}

static bool vaporize_decreasing(struct Point * * const map,
		int const max_x,
		int const max_y,
		int const x,
		int const y,
		int const direction,
		double * const last_slope) {
	int closest_distance = -1;
	double biggest_slope = -DBL_MAX;
	int x3 = 0, y3 = 0;
	for (int y2 = 0; y2 < max_y; y2++) {
		for (int x2 = 0; x2 < max_x; x2++) {
			if (x2 == x && y2 == y) {
				continue;
			}
			if (!map[x2][y2].asteroid) {
				continue;
			}
			if (map[x2][y2].vaporized) {
				continue;
			}
			if (direction != map[x2][y2].direction) {
				continue;
			}

			if (map[x2][y2].slope < *last_slope &&
					(map[x2][y2].slope > biggest_slope ||
					 double_equal(map[x2][y2].slope, biggest_slope))) {
				int const distance = calc_distance(x, y, x2, y2);
				if (double_equal(map[x2][y2].slope, biggest_slope)) {
					if (closest_distance == -1 || distance < closest_distance) {
						closest_distance = distance;
						x3 = x2;
						y3 = y2;
					}
				} else {
					closest_distance = distance;
					x3 = x2;
					y3 = y2;
				}

				biggest_slope = map[x2][y2].slope;
			}
		}
	}

	if (closest_distance == -1) {
		return false;
	}
	printf("vaporize %d,%d (answer: %d)", x3, y3, x3*100+y3);
	map[x3][y3].vaporized = true;
	*last_slope = biggest_slope;
	return true;
}

static bool vaporize_increasing(struct Point * * const map,
		int const max_x,
		int const max_y,
		int const x,
		int const y,
		int const direction,
		double * const last_slope) {
	int closest_distance = -1;
	double smallest_slope = DBL_MAX;
	int x3 = 0, y3 = 0;
	for (int y2 = 0; y2 < max_y; y2++) {
		for (int x2 = 0; x2 < max_x; x2++) {
			if (x2 == x && y2 == y) {
				continue;
			}
			if (!map[x2][y2].asteroid) {
				continue;
			}
			if (map[x2][y2].vaporized) {
				continue;
			}
			if (direction != map[x2][y2].direction) {
				continue;
			}

			if (map[x2][y2].slope > *last_slope &&
					(map[x2][y2].slope < smallest_slope ||
					 double_equal(map[x2][y2].slope, smallest_slope))) {
				int const distance = calc_distance(x, y, x2, y2);
				if (double_equal(map[x2][y2].slope, smallest_slope)) {
					if (closest_distance == -1 || distance < closest_distance) {
						closest_distance = distance;
						x3 = x2;
						y3 = y2;
					}
				} else {
					closest_distance = distance;
					x3 = x2;
					y3 = y2;
				}

				smallest_slope = map[x2][y2].slope;
			}
		}
	}

	if (closest_distance == -1) {
		return false;
	}
	printf("vaporize %d,%d (answer: %d)", x3, y3, x3*100+y3);
	map[x3][y3].vaporized = true;
	*last_slope = smallest_slope;
	return true;
}

static int calc_distance(
	int const x1,
	int const y1,
	int const x2,
	int const y2) {
	double const d = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
	return (int) d;
}
