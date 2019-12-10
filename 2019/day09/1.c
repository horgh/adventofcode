#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run(char const * const);
static void enqueue2(struct Queue * const, int64_t const);
static int64_t dequeue2(struct Queue * const);
static void run_program(
		int64_t * const,
		size_t const,
		struct Queue * const);
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

	struct Queue * const q = calloc(1, sizeof(struct Queue));
	assert(q != NULL);
	enqueue2(q, 1);
	run_program(program, n, q);
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

static void run_program(
		int64_t * const program,
		size_t const n,
		struct Queue * const q) {
	int base = 0;
	for (size_t i = 0; i < n; ) {
		int const opcode = (int) program[i]%100;

		int modes[8] = {0};
		size_t n_modes = 0;
		int raw_modes = (int) program[i]/100;
		while (raw_modes > 0) {
			modes[n_modes] = raw_modes%10;
			n_modes++;
			raw_modes /= 10;
		}

		if (opcode == 1 || opcode == 2) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			int64_t const v1 = get_value(program, i+2, modes[1], base);

			if (opcode == 1) {
				set_value(program, i+3, modes[2], base, v0+v1);
			} else {
				set_value(program, i+3, modes[2], base, v0*v1);
			}

			i += 4;
			continue;
		}

		if (opcode == 3) {
			int64_t const v0 = dequeue2(q);
			set_value(program, i+1, modes[0], base, v0);
			i += 2;
			continue;
		}

		if (opcode == 4) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			i += 2;
			printf("%" PRId64 "\n", v0);
			continue;
		}

		if (opcode == 5) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			int64_t const v1 = get_value(program, i+2, modes[1], base);
			if (v0) {
				i = (size_t) v1;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 6) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			int64_t const v1 = get_value(program, i+2, modes[1], base);
			if (v0 == 0) {
				i = (size_t) v1;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 7) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			int64_t const v1 = get_value(program, i+2, modes[1], base);
			int64_t const v2 = v0 < v1 ? 1 : 0;
			set_value(program, i+3, modes[2], base, v2);
			i += 4;
			continue;
		}

		if (opcode == 8) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			int64_t const v1 = get_value(program, i+2, modes[1], base);
			int64_t const v2 = v0 == v1 ? 1 : 0;
			set_value(program, i+3, modes[2], base, v2);
			i += 4;
			continue;
		}

		if (opcode == 9) {
			int64_t const v0 = get_value(program, i+1, modes[0], base);
			base += (int) v0;
			i += 2;
			continue;
		}

		if (opcode == 99) {
			i++;
			return;
		}

		printf("invalid opcode %d\n", opcode);
		exit(1);
	}

	printf("unexpectedly finished program\n");
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
