#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Point {
	int x;
	int y;
	int vx;
	int vy;
};

static void
move(struct Point * const,
		size_t const);
static bool
draw(struct Point const * const,
		size_t const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Point points[4096] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;

		ptr += strlen("position=<");
		while (isspace(*ptr)) {
			ptr++;
		}

		points[n].x = atoi(ptr);
		while (*ptr != ',') {
			ptr++;
		}
		ptr++;
		while (isspace(*ptr)) {
			ptr++;
		}

		points[n].y = atoi(ptr);
		while (*ptr != '>') {
			ptr++;
		}
		ptr += strlen("> velocity=<");
		while (isspace(*ptr)) {
			ptr++;
		}
		points[n].vx = atoi(ptr);
		while (*ptr != ',') {
			ptr++;
		}
		ptr++;
		while (isspace(*ptr)) {
			ptr++;
		}
		points[n].vy = atoi(ptr);
		n++;
	}

	while (1) {
		if (draw(points, n)) {
			while (1) {
				move(points, n);
				if (!draw(points, n)) {
					return 0;
				}
			}
		}
		move(points, n);
	}

	return 0;
}

static void
move(struct Point * const points,
		size_t const n)
{
	for (size_t i = 0; i < n; i++) {
		points[i].x += points[i].vx;
		points[i].y += points[i].vy;
	}
}

#define SZ 230

static bool
draw(struct Point const * const points,
		size_t const n)
{
	bool m[SZ][SZ] = {0};
	for (size_t i = 0; i < n; i++) {
		int const x = points[i].x;
		int const y = points[i].y;
		if (x < 0 || x >= SZ) {
			return false;
		}
		if (y < 0 || y >= SZ) {
			return false;
		}
		m[x][y] = true;
	}

	for (size_t y = 0; y < SZ; y++) {
		for (size_t x = 0; x < SZ; x++) {
			if (m[x][y]) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}

	return true;
}
