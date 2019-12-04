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
	int program[512] = {0};
	size_t n = 0;
	char const * ptr = buf;
	while (*ptr != '\0' && *ptr != '\n') {
		int const i = atoi(ptr);
		program[n++] = i;
		while (isdigit(*ptr)) {
			ptr++;
		}
		if (*ptr == ',') {
			ptr++;
		}
	}

	for (int i = 0; i < 99; i++) {
		for (int j = 0; j < 99; j++) {
			int program_copy[512] = {0};
			memcpy(program_copy, program, sizeof(program));

			program_copy[1] = i;
			program_copy[2] = j;

			run_program(program_copy, n);

			if (program_copy[0] == 19690720) {
				printf("%d\n", 100*i+j);
				return;
			}
		}
	}
}

static void run_program(int * const program, size_t const n) {
	for (size_t i = 0; i < n; i += 4) {
		int const input0_pos = program[i+1];
		int const input1_pos = program[i+2];
		int const output_pos = program[i+3];
		switch (program[i]) {
		case 1:
			program[output_pos] = program[input0_pos] + program[input1_pos];
			break;
		case 2:
			program[output_pos] = program[input0_pos] * program[input1_pos];
			break;
		case 99:
			return;
			break;
		default:
			return;
		}
	}
}
