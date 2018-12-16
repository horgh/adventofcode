#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#define REG_SZ 4
#define REG_TYPE uint16_t

struct Instruction {
	int op;
	REG_TYPE a;
	REG_TYPE b;
	REG_TYPE c;
};

struct Example {
	REG_TYPE before[REG_SZ];
	struct Instruction instruction;
	REG_TYPE after[REG_SZ];
};

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Example examples[1024] = {0};
	size_t n_examples = 0;
	size_t n_blanks = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;

		if (strlen(ptr) == 0) {
			n_blanks++;
			if (n_blanks == 3) {
				break;
			}
			continue;
		}
		n_blanks = 0;

		if (strncmp(ptr, "Before: [", strlen("Before: [")) == 0) {
			ptr += strlen("Before: [");
			examples[n_examples].before[0] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].before[1] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].before[2] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].before[3] = (REG_TYPE) atoi(ptr);
			continue;
		}
		if (isdigit(*ptr)) {
			examples[n_examples].instruction.op = atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].instruction.a = (REG_TYPE) atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].instruction.b = (REG_TYPE) atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].instruction.c = (REG_TYPE) atoi(ptr);
			continue;
		}

		if (strncmp(ptr, "After:  [", strlen("After:  [")) == 0) {
			ptr += strlen("After:  [");
			examples[n_examples].after[0] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].after[1] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].after[2] = (REG_TYPE) atoi(ptr);
			while (*ptr != ',') {
				ptr++;
			}
			ptr++;
			while (isspace(*ptr)) {
				ptr++;
			}
			examples[n_examples].after[3] = (REG_TYPE) atoi(ptr);
			n_examples++;
			continue;
		}
		printf("unexpected input [%s]\n", ptr);
		return 1;
	}

	if (false) {
		for (size_t i = 0; i < n_examples; i++) {
			printf("Before: [");
			for (size_t j = 0; j < 3; j++) {
				printf("%d, ", examples[i].before[j]);
			}
			printf("%d", examples[i].before[3]);
			printf("]\n");
			printf("%d %d %d %d\n", examples[i].instruction.op,
					examples[i].instruction.a, examples[i].instruction.b,
					examples[i].instruction.c);
			printf("After:  [");
			for (size_t j = 0; j < 3; j++) {
				printf("%d, ", examples[i].after[j]);
			}
			printf("%d", examples[i].after[3]);
			printf("]\n");
			printf("\n");
		}
	}

	int examples_like_3_or_more_opcodes = 0;
	for (size_t i = 0; i < n_examples; i++) {
		struct Example const * const example = &examples[i];
		struct Instruction const * const instr = &example->instruction;
		REG_TYPE regs[REG_SZ] = {0};
		int count = 0;

		// addr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = (REG_TYPE) (regs[instr->a]+regs[instr->b]);
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// addi
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = (REG_TYPE) (regs[instr->a]+instr->b);
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// mulr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = (REG_TYPE) (regs[instr->a]*regs[instr->b]);
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// muli
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = (REG_TYPE) (regs[instr->a]*instr->b);
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// banr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = regs[instr->a]&regs[instr->b];
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// bani
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = regs[instr->a]&instr->b;
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// borr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = regs[instr->a]|regs[instr->b];
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// bori
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = regs[instr->a]|instr->b;
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// setr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = regs[instr->a];
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// seti
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		regs[instr->c] = instr->a;
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// gtir
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (instr->a > regs[instr->b]) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// gtri
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (regs[instr->a] > instr->b) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// gtrr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (regs[instr->a] > regs[instr->b]) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// eqir
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (instr->a == regs[instr->b]) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// eqri
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (regs[instr->a] == instr->b) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}
		// eqrr
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		if (regs[instr->a] == regs[instr->b]) {
			regs[instr->c] = 1;
		} else {
			regs[instr->c] = 0;
		}
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			count++;
		}

		if (count >= 3) {
			examples_like_3_or_more_opcodes++;
		}
	}

	printf("%d\n", examples_like_3_or_more_opcodes);
	return 0;
}
