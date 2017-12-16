#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPIN     0
#define EXCHANGE 1
#define PARTNER  2

struct Move {
	int type;

	int amount;

	int index0;
	int index1;

	char program0;
	char program1;
};

static void
destroy_moves(struct Move * * const);
static struct Move *
parse_move(char const * const);
static bool
dance(char * const,
		struct Move * * const,
		size_t const);
static bool
perform_move(char * const,
		struct Move const * const);
static void
rotate(char * const);
static int
index(char const * const, char const);
static void
print_move(struct Move const *);

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
		char const c = (char) ('a'+(char)i);
		programs[i] = c;
	}

	size_t const max_moves = 10240;
	struct Move * * const moves = calloc(max_moves, sizeof(struct Move *));
	if (!moves) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}

	ptr = buf;
	size_t num_moves = 0;
	while (*ptr != '\0') {
		moves[num_moves] = parse_move(ptr);
		if (!moves[num_moves]) {
			fprintf(stderr, "error parsing move\n");
			destroy_moves(moves);
			return 1;
		}
		num_moves++;

		if (num_moves == max_moves) {
			fprintf(stderr, "too many moves\n");
			destroy_moves(moves);
			return 1;
		}

		while (*ptr != '\0' && *ptr != ',') {
			ptr++;
		}
		if (*ptr == ',') {
			ptr++;
		}
	}


	if (!dance(programs, moves, num_moves)) {
		fprintf(stderr, "dancing failed\n");
		destroy_moves(moves);
		return 1;
	}

	char orig[256] = {0};
	strcpy(orig, programs);

	uint64_t m = 1000000000;
	for (uint64_t i = 1; i < m; i++) {
		if (!dance(programs, moves, num_moves)) {
			fprintf(stderr, "dancing failed\n");
			destroy_moves(moves);
			return 1;
		}

		// Find a nice spot where we start to repeat to cut down our work
		if (strcmp(programs, orig) == 0) {
			m = m%i;
			// +1 when we start our loop
			i = 0;
		}
	}

	destroy_moves(moves);

	printf("%s\n", programs);
	return 0;
}

static void
destroy_moves(struct Move * * const moves)
{
	if (!moves) {
		return;
	}

	size_t i = 0;
	while (1) {
		struct Move * const move = moves[i++];
		if (!move) {
			break;
		}
		free(move);
	}

	free(moves);
}

static struct Move *
parse_move(char const * const moves)
{
	struct Move * const move = calloc(1, sizeof(struct Move));
	if (!move) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = moves;
	if (*ptr == 's') {
		move->type = SPIN;
		ptr++;
		move->amount = atoi(ptr);
		return move;
	}

	if (*ptr == 'x') {
		move->type = EXCHANGE;
		ptr++;
		move->index0 = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		if (*ptr != '/') {
			fprintf(stderr, "unexpected x move\n");
			free(move);
			return NULL;
		}
		ptr++;
		move->index1 = atoi(ptr);
		return move;
	}

	if (*ptr == 'p') {
		move->type = PARTNER;
		move->program0 = ptr[1];
		move->program1 = ptr[3];
		return move;
	}

	fprintf(stderr, "unrecognized move: %s\n", moves);
	free(move);
	return NULL;
}

static bool
dance(char * const programs,
		struct Move * * const moves,
		size_t const num_moves)
{
	for (size_t i = 0; i < num_moves; i++) {
		if (!perform_move(programs, moves[i])) {
			fprintf(stderr, "failed to perform move\n");
			return false;
		}

		if (0) {
			printf("%s after ", programs);
			print_move(moves[i]);
		}
	}
	return true;
}

static bool
perform_move(char * const programs,
		struct Move const * const move)
{
	if (move->type == SPIN) {
		for (int i = 0; i < move->amount; i++) {
			rotate(programs);
		}
		return true;
	}

	if (move->type == EXCHANGE) {
		char const tmp = programs[move->index0];
		programs[move->index0] = programs[move->index1];
		programs[move->index1] = tmp;
		return true;
	}

	if (move->type == PARTNER) {
		int const index0 = index(programs, move->program0);
		int const index1 = index(programs, move->program1);
		if (index0 == -1 || index1 == -1) {
			fprintf(stderr, "index not found: 0: %d 1: %d\n", index0, index1);
			return false;
		}

		char const tmp = programs[index0];
		programs[index0] = programs[index1];
		programs[index1] = tmp;
		return true;
	}

	fprintf(stderr, "unknown move\n");
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

static void
print_move(struct Move const * move)
{
	if (move->type == SPIN) {
		printf("spin ");
	} else if (move->type == EXCHANGE) {
		printf("exchange ");
	} else if (move->type == PARTNER) {
		printf("partner ");
	} else {
		fprintf(stderr, "unknown move\n");
	}

	printf("amount: %d index0: %d index1: %d program0: %c program1: %c\n",
			move->amount, move->index0, move->index1, move->program0, move->program1);
}
