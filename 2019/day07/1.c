#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run(char const * const);
static int run_program(
		int const * const,
		size_t const,
		int const * const);

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

	int inputs[2] = {0};
	int max = -1;
	for (int i = 0; i < 5; i++) {
		inputs[0] = i;
		inputs[1] = 0;
		int const output0 = run_program(program, n, inputs);
		for (int j = 0; j < 5; j++) {
			if (j == i) {
				continue;
			}
			inputs[0] = j;
			inputs[1] = output0;
			int const output1 = run_program(program, n, inputs);
			for (int k = 0; k < 5; k++) {
				if (k == i || k == j) {
					continue;
				}
				inputs[0] = k;
				inputs[1] = output1;
				int const output2 = run_program(program, n, inputs);
				for (int l = 0; l < 5; l++) {
					if (l == i || l == j || l == k) {
						continue;
					}
					inputs[0] = l;
					inputs[1] = output2;
					int const output3 = run_program(program, n, inputs);
					for (int b = 0; b < 5; b++) {
						if (b == i || b == j || b == k || b == l) {
							continue;
						}
						inputs[0] = b;
						inputs[1] = output3;
						int const output4 = run_program(program, n, inputs);
						if (max == -1 || output4 > max) {
							max = output4;
						}
					}
				}
			}
		}
	}

	printf("%d\n", max);
}

static int run_program(
		int const * const program_orig,
		size_t const n,
		int const * const inputs) {
	int program[1024] = {0};
	memcpy(program, program_orig, 1024);

	int inputs_count = 0;

	for (size_t i = 0; i < n; ) {
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
			int const value0 = modes[0] == 0 ?
				program[program[i+1]] : program[i+1];
			int const value1 = modes[1] == 0 ?
				program[program[i+2]] : program[i+2];

			if (opcode == 1) {
				program[program[i+3]] = value0+value1;
			} else {
				program[program[i+3]] = value0*value1;
			}

			i += 4;
			continue;
		}

		if (opcode == 3) {
			program[program[i+1]] = inputs[inputs_count++];
			i += 2;
			continue;
		}

		if (opcode == 4) {
			int const output = modes[0] == 0 ?
					program[program[i+1]] : program[i+1];
			return output;
		}

		if (opcode == 5) {
			int const v = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			if (v) {
				i = (size_t) v2;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 6) {
			int const v = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			if (v == 0) {
				i = (size_t) v2;
				continue;
			}
			i += 3;
			continue;
		}

		if (opcode == 7) {
			int const v = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			program[program[i+3]] = v < v2 ? 1 : 0;
			i += 4;
			continue;
		}

		if (opcode == 8) {
			int const v = modes[0] == 0 ? program[program[i+1]] : program[i+1];
			int const v2 = modes[1] == 0 ? program[program[i+2]] : program[i+2];
			program[program[i+3]] = v == v2 ? 1 : 0;
			i += 4;
			continue;
		}

		if (opcode == 99) {
			return -1;
		}

		printf("invalid opcode %d\n", opcode);
		abort();
	}

	return -1;
}
