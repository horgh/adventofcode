#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void run(char const * const, bool const);
static void run_program(int * const, size_t const);

int
main(int const argc, char const * const * const argv)
{
	(void) argv;
	bool override_instructions = false;
	if (argc == 2) {
		override_instructions = true;
	}

	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		run(buf, override_instructions);
	}

	return 0;
}

static void run(char const * const buf, bool const override_instructions) {
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

	if (override_instructions) {
		program[1] = 12;
		program[2] = 2;
	}

	run_program(program, n);

	for (size_t i = 0; i < n; i++) {
		if (i != 0) {
			printf(",");
		}
		printf("%d", program[i]);
	}
	printf("\n");
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
			printf("error\n");
			return;
		}
	}
}
