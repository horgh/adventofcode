#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run(char const * const);
static int amplify(
		int const * const,
		size_t const,
		int const * const);
static void enqueue2(struct Queue * const, int const);
static int dequeue2(struct Queue * const);
static int run_program(
		int * const,
		size_t const,
		int * const,
		struct Queue * const);

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
	int program[1024] = {0};
	size_t n = 0;
	char const * ptr = buf;
	while (*ptr != '\0' && *ptr != '\n') {
		int const i = atoi(ptr);
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

	int max = -1;
	for (int i = 5; i < 10; i++) {
		for (int j = 5; j < 10; j++) {
			if (j == i) {
				continue;
			}
			for (int k = 5; k < 10; k++) {
				if (k == i || k == j) {
					continue;
				}
				for (int l = 5; l < 10; l++) {
					if (l == i || l == j || l == k) {
						continue;
					}
					for (int b = 5; b < 10; b++) {
						if (b == i || b == j || b == k || b == l) {
							continue;
						}
						int phase_settings[5] = {i, j, k, l, b};
						//printf("phase settings %d, %d, %d, %d, %d\n", i, j, k, l, b);
						int const output = amplify(program, n, phase_settings);
						if (max == -1 || output > max) {
							max = output;
						}
					}
				}
			}
		}
	}

	printf("%d\n", max);
}

static int amplify(
		int const * const program_orig,
		size_t const n,
		int const * const phase_settings) {
	int program0[1024] = {0};
	memcpy(program0, program_orig, 1024*sizeof(int));
	int program1[1024] = {0};
	memcpy(program1, program_orig, 1024*sizeof(int));
	int program2[1024] = {0};
	memcpy(program2, program_orig, 1024*sizeof(int));
	int program3[1024] = {0};
	memcpy(program3, program_orig, 1024*sizeof(int));
	int program4[1024] = {0};
	memcpy(program4, program_orig, 1024*sizeof(int));
	int ips[5] = {0};
	struct Queue * const q0 = calloc(1, sizeof(struct Queue));
	assert(q0 != NULL);
	struct Queue * const q1 = calloc(1, sizeof(struct Queue));
	assert(q1 != NULL);
	struct Queue * const q2 = calloc(1, sizeof(struct Queue));
	assert(q2 != NULL);
	struct Queue * const q3 = calloc(1, sizeof(struct Queue));
	assert(q3 != NULL);
	struct Queue * const q4 = calloc(1, sizeof(struct Queue));
	assert(q4 != NULL);
	int last_e = 0;

	enqueue2(q0, phase_settings[0]);
	enqueue2(q0, 0);

	enqueue2(q1, phase_settings[1]);
	enqueue2(q2, phase_settings[2]);
	enqueue2(q3, phase_settings[3]);
	enqueue2(q4, phase_settings[4]);
	while (1) {
		int v = 0;

		v = run_program(program0, n, &ips[0], q0);
		if (v == -1) {
			return last_e;
		}
		enqueue2(q1, v);

		v = run_program(program1, n, &ips[1], q1);
		if (v == -1) {
			return last_e;
		}
		enqueue2(q2, v);

		v = run_program(program2, n, &ips[2], q2);
		if (v == -1) {
			return last_e;
		}
		enqueue2(q3, v);

		v = run_program(program3, n, &ips[3], q3);
		if (v == -1) {
			return last_e;
		}
		enqueue2(q4, v);

		v = run_program(program4, n, &ips[4], q4);
		if (v == -1) {
			return last_e;
		}
		enqueue2(q0, v);
		last_e = v;
	}
}

static void enqueue2(struct Queue * const q, int const v) {
	int * const v2 = calloc(1, sizeof(int));
	assert(v2 != NULL);
	*v2 = v;
	assert(enqueue(q, v2));
}

static int dequeue2(struct Queue * const q) {
	int * const v = dequeue(q);
	assert(v != NULL);
	int const v2 = *v;
	free(v);
	return v2;
}

static int run_program(
		int * const program,
		size_t const n,
		int * const ip,
		struct Queue * const q) {
	for (int i = *ip; i < (int) n; ) {
		int const opcode = program[i]%100;

		int modes[8] = {0};
		size_t n_modes = 0;
		int raw_modes = program[i]/100;
		while (raw_modes > 0) {
			modes[n_modes] = raw_modes%10;
			n_modes++;
			raw_modes /= 10;
		}

		if (opcode == 1 || opcode == 2) {
			int const value0 = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const value1 = modes[1] == 0 ? program[program[i+2]] : program[i+2];

			if (opcode == 1) {
				program[program[i+3]] = value0+value1;
			} else {
				program[program[i+3]] = value0*value1;
			}

			i += 4;
			continue;
		}

		if (opcode == 3) {
			int const input = dequeue2(q);
			program[program[i+1]] = input;
			i += 2;
			continue;
		}

		if (opcode == 4) {
			int const output = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			i += 2;
			*ip = i;
			return output;
		}

		if (opcode == 5) {
			int const v  = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			if (v) {
				i = v2;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 6) {
			int const v  = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			if (v == 0) {
				i = v2;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 7) {
			int const v  = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			program[program[i+3]] = v < v2 ? 1 : 0;
			i += 4;
			continue;
		}

		if (opcode == 8) {
			int const v  = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			program[program[i+3]] = v == v2 ? 1 : 0;
			i += 4;
			continue;
		}

		if (opcode == 99) {
			i++;
			*ip = i;
			return -1;
		}

		printf("invalid opcode %d\n", opcode);
		exit(1);
	}

	printf("unexpectedly finished program\n");
	abort();
	return -1;
}
