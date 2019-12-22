#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Moon {
	int x;
	int y;
	int z;
	int vx;
	int vy;
	int vz;
};

static uint64_t solve(int const * const,
	int * const,
	int const * const,
	int * const);
static void step(int * const, int * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Moon moons[8] = {0};
	size_t n_moons = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		char const * ptr = buf;
		ptr += strlen("<x=");
		moons[n_moons].x = atoi(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr += strlen(", y=");
		moons[n_moons].y = atoi(ptr);
		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		ptr += strlen(", z=");
		moons[n_moons].z = atoi(ptr);
		n_moons++;
	}

	int ox[4] = {moons[0].x, moons[1].x, moons[2].x, moons[3].x};
	int oy[4] = {moons[0].y, moons[1].y, moons[2].y, moons[3].y};
	int oz[4] = {moons[0].z, moons[1].z, moons[2].z, moons[3].z};

	int x[4] = {0};
	memcpy(x, ox, sizeof(ox));
	int y[4] = {0};
	memcpy(y, oy, sizeof(oy));
	int z[4] = {0};
	memcpy(z, oz, sizeof(oz));

	int ov[4] = {0, 0, 0, 0};

	int vx[4] = {0, 0, 0, 0};
	int vy[4] = {0, 0, 0, 0};
	int vz[4] = {0, 0, 0, 0};

	uint64_t const xs = solve(ox, x, ov, vx);
	uint64_t const ys = solve(oy, y, ov, vy);
	uint64_t const zs = solve(oz, z, ov, vz);

	uint64_t smallest = xs;
	if (ys < smallest) {
		smallest = xs;
	}
	if (zs < smallest) {
		smallest = zs;
	}
	uint64_t steps = 0;
	while (1) {
		steps += smallest;
		if (steps % xs == 0 &&
				steps % ys == 0 &&
				steps % zs == 0) {
			break;
		}
	}
	printf("%" PRIu64 "\n", steps);

	return 0;
}

static uint64_t solve(int const * const op,
	int * const p,
	int const * const ov,
	int * const v) {
	uint64_t steps = 0;
	while (1) {
		step(p, v);
		steps++;
		if (memcmp(op, p, sizeof(int)*4) == 0 &&
				memcmp(ov, v, sizeof(int)*4) == 0) {
			return steps;
		}
	}
}

static void step(int * const p, int * const v) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				continue;
			}
			if (p[i] > p[j]) {
				v[i]--;
			} else if (p[i] < p[j]) {
				v[i]++;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		p[i] += v[i];
	}
}
