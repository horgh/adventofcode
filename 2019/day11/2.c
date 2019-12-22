#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SZ 128

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define BLACK 0
#define WHITE 1

#define TURN_LEFT  0
#define TURN_RIGHT 1

struct Panel {
	int colour;
	bool painted;
};

static void run(char const * const);
static void enqueue2(struct Queue * const, int64_t const);
static int64_t dequeue2(struct Queue * const);
static int64_t run_program(
		int64_t * const,
		size_t const,
		size_t * const,
		int * const,
		struct Queue * const,
		bool * const);
static int64_t get_value(
		int64_t const * const,
		size_t const,
		int const,
		int const);
static void set_value(
		int64_t * const,
		size_t const,
		int const,
		int const,
		int64_t const);
static void move(size_t * const,
		size_t * const,
		int * const,
		int const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		run(buf);
	}

	return 0;
}

static void run(char const * const buf) {
	int64_t program[102400] = {0};
	size_t n = 0;
	char const * ptr = buf;
	while (*ptr != '\0' && *ptr != '\n') {
		int64_t const i = atoll(ptr);
		program[n] = i;
		n++;
		if (n == sizeof(program)) {
			printf("maxed out instructions\n");
			abort();
		}

		while (isdigit(*ptr) || *ptr == '-') {
			ptr++;
		}
		if (*ptr == ',') {
			ptr++;
		}
	}

	struct Panel * * const map = calloc(MAP_SZ, sizeof(struct Panel *));
	assert(map != NULL);
	for (size_t i = 0; i < MAP_SZ; i++) {
		map[i] = calloc(MAP_SZ, sizeof(struct Panel));
		assert(map[i] != NULL);
	}

	struct Queue * const q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);

	bool done = false;
	size_t ip = 0;
	int base = 0;
	int direction = UP;
	size_t x = MAP_SZ/2, y = MAP_SZ/2;
	map[x][y].colour = WHITE;
	while (1) {
		enqueue2(q, map[x][y].colour);

		int64_t const colour = run_program(program, n, &ip, &base, q, &done);
		if (done) {
			break;
		}
		map[x][y].colour = (int) colour;
		map[x][y].painted = true;

		int64_t const turn = run_program(program, n, &ip, &base, q, &done);
		if (done) {
			break;
		}
		move(&x, &y, &direction, (int) turn);
	}

	for (size_t y2 = 0; y2 < MAP_SZ; y2++) {
		for (size_t x2 = 0; x2 < MAP_SZ; x2++) {
			if (map[x2][y2].colour == BLACK) {
				printf(".");
			} else {
				printf("#");
			}
		}
		printf("\n");
	}
}

static void enqueue2(struct Queue * const q, int64_t const v) {
	int64_t * const v2 = calloc(1, sizeof(int64_t));
	assert(v2 != NULL);
	*v2 = v;
	assert(enqueue(q, v2));
}

static int64_t dequeue2(struct Queue * const q) {
	int64_t * const v = dequeue(q);
	assert(v != NULL);
	int64_t const v2 = *v;
	free(v);
	return v2;
}

static int64_t run_program(
		int64_t * const program,
		size_t const n,
		size_t * const ip,
		int * const base,
		struct Queue * const q,
		bool * const done) {
	for (size_t i = *ip; i < n; ) {
		int const opcode = (int) program[i]%100;

		int modes[8] = {0};
		size_t n_modes = 0;
		int raw_modes = (int) program[i]/100;
		while (raw_modes > 0) {
			assert(n_modes != 8);
			modes[n_modes] = raw_modes%10;
			n_modes++;
			raw_modes /= 10;
		}

		if (opcode == 1 || opcode == 2) { // Add or multiply
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			int64_t const v1 = get_value(program, i+2, modes[1], *base);

			if (opcode == 1) {
				set_value(program, i+3, modes[2], *base, v0+v1);
			} else {
				set_value(program, i+3, modes[2], *base, v0*v1);
			}

			i += 4;
			continue;
		}

		if (opcode == 3) { // Input
			int64_t const v0 = dequeue2(q);
			set_value(program, i+1, modes[0], *base, v0);
			i += 2;
			continue;
		}

		if (opcode == 4) { // Output
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			i += 2;
			*ip = i;
			return v0;
		}

		if (opcode == 5) { // Jump if true
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			int64_t const v1 = get_value(program, i+2, modes[1], *base);
			if (v0) {
				i = (size_t) v1;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 6) { // Jump if false
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			int64_t const v1 = get_value(program, i+2, modes[1], *base);
			if (v0 == 0) {
				i = (size_t) v1;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 7) { // Less than
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			int64_t const v1 = get_value(program, i+2, modes[1], *base);
			int64_t const v2 = v0 < v1 ? 1 : 0;
			set_value(program, i+3, modes[2], *base, v2);
			i += 4;
			continue;
		}

		if (opcode == 8) { // Equals
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			int64_t const v1 = get_value(program, i+2, modes[1], *base);
			int64_t const v2 = v0 == v1 ? 1 : 0;
			set_value(program, i+3, modes[2], *base, v2);
			i += 4;
			continue;
		}

		if (opcode == 9) { // Adjust relative base for relative mode
			int64_t const v0 = get_value(program, i+1, modes[0], *base);
			*base += (int) v0;
			i += 2;
			continue;
		}

		if (opcode == 99) { // Stop
			i++;
			*ip = i;
			*done = true;
			return -1;
		}

		printf("invalid opcode %d\n", opcode);
		exit(1);
	}

	printf("unexpectedly finished program\n");
	exit(1);
}

__attribute__((pure))
static int64_t get_value(
		int64_t const * const program,
		size_t const ip,
		int const mode,
		int const base) {
	if (mode == 0) {
		return program[program[ip]];
	}
	if (mode == 1) {
		return program[ip];
	}
	if (mode == 2) {
		return program[(int) program[ip]+base];
	}
	printf("invalid mode\n");
	exit(1);
	return -1;
}

static void set_value(
		int64_t * const program,
		size_t const ip,
		int const mode,
		int const base,
		int64_t const value)
{
	if (mode == 0) {
		program[program[ip]] = value;
		return;
	}
	if (mode == 1) {
		printf("invalid - immediate mode when setting value\n");
		exit(1);
	}
	if (mode == 2) {
		program[program[ip]+base] = value;
		return;
	}
	printf("invalid mode\n");
	exit(1);
}

static void move(size_t * const x,
		size_t * const y,
		int * const direction,
		int const turn)
{
	if (turn == TURN_LEFT) {
		switch (*direction) {
		case UP:
			*direction = LEFT;
			break;
		case RIGHT:
			*direction = UP;
			break;
		case DOWN:
			*direction = RIGHT;
			break;
		case LEFT:
			*direction = DOWN;
			break;
		default:
			printf("unexpected direction\n");
			exit(1);
		}
	}
	if (turn == TURN_RIGHT) {
		switch (*direction) {
		case UP:
			*direction = RIGHT;
			break;
		case RIGHT:
			*direction = DOWN;
			break;
		case DOWN:
			*direction = LEFT;
			break;
		case LEFT:
			*direction = UP;
			break;
		default:
			printf("unexpected direction\n");
			exit(1);
		}
	}

	switch (*direction) {
	case UP:
		assert(*y != 0);
		*y -= 1;
		break;
	case RIGHT:
		assert(*x + 1 < MAP_SZ);
		*x += 1;
		break;
	case DOWN:
		assert(*y + 1 < MAP_SZ);
		*y += 1;
		break;
	case LEFT:
		assert(*x != 0);
		*x -= 1;
		break;
	default:
		printf("unexpected direction");
		exit(1);
	}
}
