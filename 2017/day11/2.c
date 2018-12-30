#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NW 0
#define N 1
#define NE 2
#define SE 3
#define S 4
#define SW 5

static int get_move(char const * const);
static int distance(int const, int const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	char buf[41960] = {0};

	if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
		fprintf(stderr, "fgets(): %s\n", strerror(errno));
		return 1;
	}

	char const * ptr = buf;
	int north = 0;
	int east = 0;
	int max_steps = 0;
	while (*ptr != '\0' && *ptr != '\n') {
		int const move = get_move(ptr);
		if (move == NW) {
			north += 1;
			east -= 1;
		} else if (move == N) {
			north += 2;
		} else if (move == NE) {
			north += 1;
			east += 1;
		} else if (move == SE) {
			north -= 1;
			east += 1;
		} else if (move == S) {
			north -= 2;
		} else if (move == SW) {
			north -= 1;
			east -= 1;
		} else {
			fprintf(stderr, "unrecognized move: %d\n", move);
			return 1;
		}

		int const steps = distance(north, east);
		if (steps > max_steps) {
			max_steps = steps;
		}

		while (isalpha(*ptr)) {
			ptr++;
		}
		if (*ptr == ',') {
			ptr++;
		}
	}

	printf("%d\n", max_steps);
	return 0;
}

static int
get_move(char const * const s)
{
	if (strncmp(s, "nw", 2) == 0) {
		return NW;
	}
	if (strncmp(s, "ne", 2) == 0) {
		return NE;
	}
	if (strncmp(s, "n", 1) == 0) {
		return N;
	}
	if (strncmp(s, "sw", 2) == 0) {
		return SW;
	}
	if (strncmp(s, "se", 2) == 0) {
		return SE;
	}
	if (strncmp(s, "s", 1) == 0) {
		return S;
	}
	fprintf(stderr, "unrecognized input: %c\n", *s);
	return -1;
}

static int
distance(int const n, int const e)
{
	int north = abs(n);
	int east = abs(e);

	int moves = 0;
	while (north || east) {
		if (north && east) {
			moves++;
			north--;
			east--;
			continue;
		}
		if (north) {
			moves++;
			north -= 2;
			continue;
		}
		if (east) {
			moves++;
			east -= 2;
			continue;
		}
	}

	return moves;
}
