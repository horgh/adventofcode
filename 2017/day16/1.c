#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
dance(char * const, char const * const);
static char const *
parse_move(char const * const, char * const);
static bool
perform_move(char * const, char const * const);
static void
rotate(char * const);
static int
index(char const * const, char const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <# of programs>\n", argv[0]);
		return 1;
	}
	int const n = atoi(argv[1]);

	FILE * const fh = stdin;

	char buf[512000] = {0};

	if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			fprintf(stderr, "fgets(): %s\n", strerror(errno));
			return 1;
	}

	char * ptr = buf;
	while (*ptr != '\0') {
		if (*ptr == '\n') {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

	char programs[256] = {0};
	for (int i = 0; i < n; i++) {
		programs[i] = (char) ('a'+(char)i);
	}

	if (!dance(programs, buf)) {
		fprintf(stderr, "dancing failed\n");
		return 1;
	}

	printf("%s\n", programs);
	return 0;
}

static bool
dance(char * const programs, char const * const moves)
{
	char const * ptr = moves;
	char move[256] = {0};
	while (*ptr != '\0') {
		memset(move, 0, sizeof(move));
		ptr = parse_move(ptr, move);
		if (!perform_move(programs, move)) {
			fprintf(stderr, "failed to perform move: %s\n", move);
			return false;
		}
	}
	return true;
}

static char const *
parse_move(char const * const moves, char * const move)
{
	char const * ptr = moves;

	size_t i = 0;
	while (*ptr != '\0' && *ptr != ',') {
		move[i++] = *ptr;
		ptr++;
	}

	if (*ptr == ',') {
		ptr++;
	}

	return ptr;
}

static bool
perform_move(char * const programs, char const * const move)
{
	if (move[0] == 's') {
		int const size = atoi(move+1);
		for (int i = 0; i < size; i++) {
			rotate(programs);
		}
		return true;
	}

	if (move[0] == 'x') {
		char const * ptr = move+1;
		int const a = atoi(ptr);

		while (isdigit(*ptr)) {
			ptr++;
		}

		if (*ptr != '/') {
			fprintf(stderr, "unexpected x move: %s\n", move);
			return false;
		}
		ptr++;

		int const b = atoi(ptr);

		char const tmp = programs[a];
		programs[a] = programs[b];
		programs[b] = tmp;
		return true;
	}

	if (move[0] == 'p') {
		char const program_a = move[1];
		char const program_b = move[3];

		int const a_index = index(programs, program_a);
		int const b_index = index(programs, program_b);
		if (a_index == -1 || b_index == -1) {
			fprintf(stderr, "index not found: a: %d b: %d\n", a_index, b_index);
			return false;
		}

		char const tmp = programs[a_index];
		programs[a_index] = programs[b_index];
		programs[b_index] = tmp;
		return true;
	}

	fprintf(stderr, "unknown move: %s\n", move);
	return false;
}

static void
rotate(char * const s)
{
	size_t const sz = strlen(s);
	char const tmp = s[sz-1];
	memcpy(s+1, s, strlen(s)-1);
	s[0] = tmp;
}

static int
index(char const * const s, char const c)
{
	char const * ptr = s;
	int i = 0;
	while (*ptr != '\0') {
		if (*ptr == c) {
			return i;
		}
		i++;
		ptr++;
	}
	return -1;
}
