#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Coordinate {
	int x;
	int y;
};

static int
distance(
		int const,
		int const,
		int const,
		int const
		);

#define SZ 512

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Coordinate coordinates[1024] = {0};
	size_t n = 0;
	while (1) {
		char buf[409600] = {0};
		if (fgets(buf, 409600, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		coordinates[n].x = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += 2;
		coordinates[n].y = atoi(ptr);
		n++;
	}

	int m[SZ][SZ] = {0};
	for (size_t x = 0; x < SZ; x++) {
		for (size_t y = 0; y < SZ; y++) {
			int distances[1024] = {0};
			for (size_t i = 0; i < n; i++) {
				distances[i] = distance(
						(int) x,
						(int) y,
						coordinates[i].x,
						coordinates[i].y
						);
			}

			int shortest = distances[0];
			size_t shortest_id = 0;
			for (size_t i = 0; i < n; i++) {
				if (distances[i] >= shortest) {
					continue;
				}
				shortest = distances[i];
				shortest_id = i;
			}

			for (size_t i = 0; i < n; i++) {
				if (distances[i] == shortest && i != shortest_id) {
					shortest = -1;
				}
			}

			if (shortest == -1) {
				m[x][y] = -1;
				continue;
			}
			m[x][y] = (int) shortest_id;
		}
	}

	int areas[1024] = {0};
	bool infinites[1024] = {0};
	for (size_t y = 0; y < SZ; y++) {
		for (size_t x = 0; x < SZ; x++) {

			if (m[x][y] == -1) {
				//printf(".");
			} else {
				//printf("%d", m[x][y]);
				if (x == 0 || x == SZ-1 || y == 0 || y == SZ-1) {
					infinites[m[x][y]] = true;
					continue;
				}
				areas[m[x][y]]++;
			}
		}
	}

	int max = 0;
	for (size_t i = 0; i < 1024; i++) {
		if (infinites[i]) {
			continue;
		}
		if (areas[i] <= max) {
			continue;
		}
		max = areas[i];
	}

	printf("%d\n", max);
	return 0;
}

static int
distance(
		int const x_0,
		int const y_0,
		int const x_1,
		int const y_1
		) {
	return abs(x_0-x_1)+abs(y_0-y_1);
}
