#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Point {
	int x;
	int y;
	int z;
	int t;
};

#define SZ 256

struct Constellation {
	int id;
	struct Point const * points[SZ];
	size_t n_points;
};

static bool constellations_linked(
		struct Constellation const * const, struct Constellation const * const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Point points[1306] = {0};
	size_t n_points = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		points[n_points].x = atoi(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr++;
		points[n_points].y = atoi(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr++;
		points[n_points].z = atoi(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr++;
		points[n_points].t = atoi(ptr);
		n_points++;
	}

	struct Constellation constellations[2048] = {0};
	size_t n_constellations = 0;
	for (size_t i = 0; i < n_points; i++) {
		struct Point const * const p = &points[i];
		constellations[n_constellations].points[0] = p;
		constellations[n_constellations].n_points++;
		n_constellations++;
	}

	size_t last = n_constellations;
	while (1) {
		for (size_t i = 0; i < n_constellations; i++) {
			struct Constellation * const c = &constellations[i];
			for (size_t j = i + 1; j < n_constellations; j++) {
				struct Constellation const * const c2 = &constellations[j];
				if (!constellations_linked(c, c2)) {
					continue;
				}
				for (size_t k = 0; k < c2->n_points; k++) {
					c->points[c->n_points++] = c2->points[k];
					assert(c->n_points != SZ);
				}
				for (size_t k = j + 1; k < n_constellations; k++) {
					constellations[k - 1] = constellations[k];
				}
				n_constellations--;
			}
		}
		if (last == n_constellations) {
			break;
		}
		last = n_constellations;
	}
	printf("%zu\n", last);
	return 0;
}

__attribute__((pure)) static bool
constellations_linked(
		struct Constellation const * const a, struct Constellation const * const b)
{
	for (size_t i = 0; i < a->n_points; i++) {
		struct Point const * const p = a->points[i];
		for (size_t j = 0; j < b->n_points; j++) {
			struct Point const * const p2 = b->points[j];
			int const dist = abs(p2->x - p->x) + abs(p2->y - p->y) +
											 abs(p2->z - p->z) + abs(p2->t - p->t);
			if (dist <= 3) {
				return true;
			}
		}
	}
	return false;
}
