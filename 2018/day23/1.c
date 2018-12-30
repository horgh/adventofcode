#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
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

static struct Bot const * find_strongest_bot(
		struct Bot const * const, size_t const);

static bool in_range(struct Bot const * const, struct Bot const * const);

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

	struct Bot const * const strongest_bot = find_strongest_bot(bots, n_bots);
	assert(strongest_bot != NULL);
	if (0) {
		printf("strongest bot has radius %" PRId64 "\n", strongest_bot->radius);
	}

	int count = 0;
	for (size_t i = 0; i < n_bots; i++) {
		if (!in_range(strongest_bot, &bots[i])) {
			continue;
		}
		count++;
	}
	printf("%d\n", count);
	return 0;
}

static struct Bot const *
find_strongest_bot(struct Bot const * const bots, size_t const n_bots)
{
	struct Bot const * strongest = NULL;
	for (size_t i = 0; i < n_bots; i++) {
		if (!strongest) {
			strongest = &bots[i];
		}
		if (bots[i].radius > strongest->radius) {
			strongest = &bots[i];
		}
	}
	return strongest;
}

static bool
in_range(struct Bot const * const b0, struct Bot const * const b1)
{
	int64_t const distance =
			llabs(b0->x - b1->x) + llabs(b0->y - b1->y) + llabs(b0->z - b1->z);
	return distance <= b0->radius;
}
