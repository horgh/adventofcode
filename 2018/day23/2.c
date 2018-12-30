#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Bot {
	int64_t x;
	int64_t y;
	int64_t z;
	int64_t radius;
};

static int64_t mymin(int64_t const, int64_t const);

static int64_t mymax(int64_t const, int64_t const);

static int count_in_range(struct Bot const * const,
		size_t const,
		int64_t const,
		int64_t const,
		int64_t const,
		int64_t const);

static int64_t distance(
		struct Bot const * const, int64_t const, int64_t const, int64_t const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Bot bots[1024] = {0};
	size_t n_bots = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		ptr += strlen("pos=<");
		bots[n_bots].x = atoll(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr++;
		bots[n_bots].y = atoll(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr++;
		bots[n_bots].z = atoll(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		assert(strncmp(ptr, ">, r=", strlen(">, r=")) == 0);
		ptr += strlen(">, r=");
		bots[n_bots].radius = atoll(ptr);
		n_bots++;
	}

	if (0) {
		for (size_t i = 0; i < n_bots; i++) {
			printf("pos=<%" PRId64 ",%" PRId64 ",%" PRId64 ">, r=%" PRId64 "\n",
					bots[i].x,
					bots[i].y,
					bots[i].z,
					bots[i].radius);
		}
	}

	int64_t min_x = 0;
	int64_t max_x = 0;
	int64_t min_y = 0;
	int64_t max_y = 0;
	int64_t min_z = 0;
	int64_t max_z = 0;
	for (size_t i = 0; i < n_bots; i++) {
		min_x = mymin(min_x, bots[i].x);
		max_x = mymax(max_x, bots[i].x);
		min_y = mymin(min_y, bots[i].y);
		max_y = mymax(max_y, bots[i].y);
		min_z = mymin(min_z, bots[i].z);
		max_z = mymax(max_z, bots[i].z);
	}

	int64_t x_start = min_x;
	int64_t x_end = max_x;
	int64_t y_start = min_y;
	int64_t y_end = max_y;
	int64_t z_start = min_z;
	int64_t z_end = max_z;

	int64_t incr = 100000000;
	while (1) {
		int best = 0;
		int64_t best_x = -1;
		int64_t best_y = -1;
		int64_t best_z = -1;
		for (int64_t x = x_start; x <= x_end; x += incr) {
			for (int64_t y = y_start; y <= y_end; y += incr) {
				for (int64_t z = z_start; z <= z_end; z += incr) {
					int const count = count_in_range(bots, n_bots, incr, x, y, z);
					if (count <= best) {
						continue;
					}
					best = count;
					best_x = x;
					best_y = y;
					best_z = z;
					continue;
				}
			}
		}

		x_start = best_x - incr;
		x_end = best_x + incr;
		y_start = best_y - incr;
		y_end = best_y + incr;
		z_start = best_z - incr;
		z_end = best_z + incr;

		incr /= 2;
		if (incr <= 1) {
			printf("%" PRId64 "\n", best_x + best_y + best_z);
			break;
		}
	}
	return 0;
}

static int64_t
mymin(int64_t const cur, int64_t const b)
{
	if (b < cur) {
		return b;
	}
	return cur;
}

static int64_t
mymax(int64_t const cur, int64_t const b)
{
	if (b > cur) {
		return b;
	}
	return cur;
}

static int
count_in_range(struct Bot const * const bots,
		size_t const n_bots,
		int64_t const incr,
		int64_t const x,
		int64_t const y,
		int64_t const z)
{
	int count = 0;
	for (size_t i = 0; i < n_bots; i++) {
		int64_t const d = distance(&bots[i], x, y, z);
		if (d - bots[i].radius >= incr) {
			continue;
		}
		count++;
	}
	return count;
}

static int64_t
distance(struct Bot const * const bot,
		int64_t const x,
		int64_t const y,
		int64_t const z)
{
	return llabs(bot->x - x) + llabs(bot->y - y) + llabs(bot->z - z);
}
