#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
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
		int * const,
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

	int scores[1024] = {0};
	int circle[102400] = {0};
	int circle_sz = 0;
	int current_pos = 0;
	int current_player = 0;
	for (int i = 0; i <= last_marble; i++) {
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

	int max = 0;
	for (int i = 1; i <= players; i++) {
		if (scores[i] > max) {
			max = scores[i];
		}
	}

	printf("%d\n", max);
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
	if (circle_sz == 0) {
		new_pos = 0;
	}
	if (circle_sz == 1) {
		new_pos = 1;
	}
	if (circle_sz == 2) {
		new_pos = 1;
	}

	if (current_pos < circle_sz-1) {
		new_pos = current_pos+2;
	} else if (current_pos == circle_sz-1) {
		new_pos = 1;
	}

	int * const circle2 = calloc((size_t) circle_sz+1, sizeof(int));
	assert(circle2 != NULL);

	for (int i = 0; i < new_pos; i++) {
		circle2[i] = circle[i];
	}
	circle2[new_pos] = marble;
	for (int i = new_pos; i < circle_sz; i++) {
		circle2[i+1] = circle[i];
	}

	for (int i = 0; i <= circle_sz; i++) {
		circle[i] = circle2[i];
	}

	free(circle2);
	return new_pos;
}

static int
place_special_marble(
		int * const scores,
		int * const circle,
		int const circle_sz,
		int const current_pos,
		int const current_player,
		int const marble)
{
	scores[current_player] += marble;

	int new_pos = current_pos;
	for (int i = 0; i < 7; i++) {
		new_pos--;
		if (new_pos == -1) {
			new_pos = circle_sz-1;
		}
	}

	scores[current_player] += circle[new_pos];

	for (int i = new_pos+1; i < circle_sz; i++) {
		circle[i-1] = circle[i];
	}

	return new_pos;
}
