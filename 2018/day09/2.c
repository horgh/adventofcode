#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

static int
place_marble(
		int * const,
		int const,
		int const,
		int const);

static int
place_special_marble(
		uint64_t * const,
		int * const,
		int const,
		int const,
		int const,
		int const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char buf[4096] = {0};
	assert(fgets(buf, 4096, stdin) != NULL);
	trim_right(buf);

	char const * ptr = buf;

	int const players = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	ptr += strlen(" players; last marble is worth ");
	int const last_marble = atoi(ptr);

	uint64_t scores[1024] = {0};
	int * const circle = calloc((size_t) last_marble+1, sizeof(int));
	assert(circle != NULL);
	int circle_sz = 0;
	int current_pos = 0;
	int current_player = 0;

	circle[0] = 0;
	circle[1] = 2;
	circle[2] = 1;
	circle_sz = 3;
	current_pos = 1;
	current_player = 3;

	for (int i = 3; i <= last_marble; i++) {
		if (i > 0 && i % 23 == 0) {
			current_pos = place_special_marble(scores, circle, circle_sz,
					current_pos, current_player, i);
			circle_sz--;
		} else {
			current_pos = place_marble(circle, circle_sz, current_pos, i);
			circle_sz++;
		}

		current_player++;
		if (current_player > players) {
			current_player = 1;
		}
	}

	free(circle);

	uint64_t max = 0;
	for (int i = 1; i <= players; i++) {
		if (scores[i] > max) {
			max = scores[i];
		}
	}

	printf("%" PRIu64 "\n", max);
	return 0;
}

static int
place_marble(
		int * const circle,
		int const circle_sz,
		int const current_pos,
		int const marble)
{
	int new_pos = 0;
	if (current_pos < circle_sz-1) {
		new_pos = current_pos+2;
	} else if (current_pos == circle_sz-1) {
		new_pos = 1;
	}

	if (new_pos < circle_sz) {
		size_t const n = sizeof(int) * (size_t) (circle_sz-new_pos);
		memcpy(circle+new_pos+1, circle+new_pos, n);
	}
	circle[new_pos] = marble;

	return new_pos;
}

static int
place_special_marble(
		uint64_t * const scores,
		int * const circle,
		int const circle_sz,
		int const current_pos,
		int const current_player,
		int const marble)
{
	scores[current_player] += (uint64_t) marble;

	int new_pos = current_pos-7;
	if (new_pos < 0) {
		new_pos = circle_sz+new_pos;
	}

	scores[current_player] += (uint64_t) circle[new_pos];

	size_t const n = sizeof(int) * (size_t) (circle_sz-new_pos-1);
	memcpy(circle+new_pos, circle+new_pos+1, n);

	return new_pos;
}
