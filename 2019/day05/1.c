#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run(char const * const);
static void run_program(int * const, size_t const);

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

	run_program(program, n);
}

static void run_program(
		int * const program,
		size_t const n) {
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
			program[program[i+1]] = 1;
			i += 2;
			continue;
		}

		if (opcode == 4) {
			int const output = modes[0] == 0 ?
					program[program[i+1]] : program[i+1];
			if (output != 0) {
				printf("%d\n", output);
			}
			i += 2;
			continue;
		}

		if (opcode == 99) {
			return;
		}

		printf("invalid opcode %d\n", opcode);
		abort();
	}
}
