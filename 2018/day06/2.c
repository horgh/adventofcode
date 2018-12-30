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

static int distance(int const, int const, int const, int const);

#define SZ 512

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <max size>\n", argv[0]);
		return 1;
	}
	int const max_size = atoi(argv[1]);

	struct Coordinate coordinates[1024] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
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

	int total_distances[SZ][SZ] = {0};
	for (size_t x = 0; x < SZ; x++) {
		for (size_t y = 0; y < SZ; y++) {
			for (size_t i = 0; i < n; i++) {
				total_distances[x][y] +=
						distance((int)x, (int)y, coordinates[i].x, coordinates[i].y);
			}
		}
	}

	int count = 0;
	for (size_t y = 0; y < SZ; y++) {
		for (size_t x = 0; x < SZ; x++) {
			if (total_distances[x][y] < max_size) {
				count++;
			}
		}
	}

	printf("%d\n", count);
	return 0;
}

static int
distance(int const x_0, int const y_0, int const x_1, int const y_1)
{
	return abs(x_0 - x_1) + abs(y_0 - y_1);
}
