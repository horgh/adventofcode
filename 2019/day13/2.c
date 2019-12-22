#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SZ 64

#define EMPTY  0
#define WALL   1
#define BLOCK  2
#define PADDLE 3
#define BALL   4

struct Tile {
	int type;
};

static void run(char const * const);
static int64_t run_program(int64_t * const,
		size_t const,
		size_t * const,
		int * const,
		int const,
		bool * const);
static int64_t get_value(int64_t const * const,
		size_t const,
		int const,
		int const);
static void set_value(int64_t * const,
		size_t const,
		int const,
		int const,
		int64_t const);

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

	struct Tile * * const map = calloc(MAP_SZ, sizeof(struct Tile *));
	assert(map != NULL);
	for (size_t i = 0; i < MAP_SZ; i++) {
		map[i] = calloc(MAP_SZ, sizeof(struct Tile));
		assert(map[i] != NULL);
	}

	struct Queue * const q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);

	program[0] = 2;

	bool done = false;
	size_t ip = 0;
	int base = 0;
	int input = 0;
	int paddle_x = 0, ball_x = 0;
	int score = 0;
	while (1) {
		int64_t const x = run_program(program, n, &ip, &base, input, &done);
		if (done) {
			break;
		}
		int64_t const y = run_program(program, n, &ip, &base, input, &done);
		if (done) {
			break;
		}
		int64_t const type = run_program(program, n, &ip, &base, input, &done);
		if (done) {
			break;
		}
		if (x == -1 && y == 0) {
			score = (int) type;
			continue;
		}
		if (x < 0 || x >= MAP_SZ || y < 0 || y >= MAP_SZ) {
			printf("invalid coords %" PRId64 ",%" PRId64 "\n", x, y);
			exit(1);
		}
		map[x][y].type = (int) type;

		if (type == PADDLE) {
			paddle_x = (int) x;
		}
		if (type == BALL) {
			ball_x = (int) x;
		}

		if (paddle_x < ball_x) {
			input = 1;
		} else if (paddle_x > ball_x) {
			input = -1;
		} else {
			input = 0;
		}

#if 0
		printf("ROUND:\n");
		for (size_t y2 = 0; y2 < MAP_SZ; y2++) {
			for (size_t x2 = 0; x2 < MAP_SZ; x2++) {
				switch (map[x2][y2].type) {
				case EMPTY:
					printf(" ");
					break;
				case WALL:
					printf("|");
					break;
				case BLOCK:
					printf("#");
					break;
				case PADDLE:
					printf("-");
					break;
				case BALL:
					printf("o");
					break;
				default:
					printf("unexpected type\n");
					exit(1);
				}
			}
			printf("\n");
		}
#endif
	}
	printf("%d\n", score);
}

static int64_t run_program(int64_t * const program,
		size_t const n,
		size_t * const ip,
		int * const base,
		int const input,
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
			int64_t const v0 = (int64_t) input;
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
static int64_t get_value(int64_t const * const program,
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

static void set_value(int64_t * const program,
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
