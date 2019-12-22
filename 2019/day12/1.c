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

static void run(struct Moon * const, size_t const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <steps>\n", argv[0]);
		return 1;
	}
	int const wanted_steps = atoi(argv[1]);

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

	for (int step = 0; step < wanted_steps; step++) {
		if (false) {
			printf("After %d steps:\n", step);
			for (size_t i = 0; i < n_moons; i++) {
				printf("<x=%d, y=%d, z=%d>, vel=<x= %d, y= %d, z= %d>\n",
						moons[i].x, moons[i].y, moons[i].z,
						moons[i].vx, moons[i].vy, moons[i].vz);
			}
		}

		run(moons, n_moons);
	}

	int energy = 0;
	for (size_t i = 0; i < n_moons; i++) {
		int const pot = abs(moons[i].x) + abs(moons[i].y) + abs(moons[i].z);
		int const kin = abs(moons[i].vx) + abs(moons[i].vy) + abs(moons[i].vz);
		energy += pot * kin;
	}
	printf("%d\n", energy);

	return 0;
}

static void run(struct Moon * const moons, size_t const n_moons) {
	for (size_t i = 0; i < n_moons; i++) {
		for (size_t j = 0; j < n_moons; j++) {
			if (i == j) {
				continue;
			}
			if (moons[i].x > moons[j].x) {
				moons[i].vx--;
			}
			if (moons[i].x < moons[j].x) {
				moons[i].vx++;
			}

			if (moons[i].y > moons[j].y) {
				moons[i].vy--;
			}
			if (moons[i].y < moons[j].y) {
				moons[i].vy++;
			}

			if (moons[i].z > moons[j].z) {
				moons[i].vz--;
			}
			if (moons[i].z < moons[j].z) {
				moons[i].vz++;
			}
		}
	}

	for (size_t i = 0; i < n_moons; i++) {
		moons[i].x += moons[i].vx;
		moons[i].y += moons[i].vy;
		moons[i].z += moons[i].vz;
	}
}
