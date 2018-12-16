#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#define REG_SZ 4
#define REG_TYPE uint16_t

enum Op {
	UNKNOWN,
	ADDR, ADDI,
	MULR, MULI,
	BANR, BANI,
	BORR, BORI,
	SETR, SETI,
	GTIR, GTRI, GTRR,
	EQIR, EQRI, EQRR
};

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

static bool
op_satisfies_examples(struct Example const * const examples,
		size_t const n_examples,
		int const opcode,
		void (*fn)(REG_TYPE * const, struct Instruction const * const));

static void
op_addr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_addi(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_mulr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_muli(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_banr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_bani(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_borr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_bori(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_setr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_seti(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_gtir(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_gtri(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_gtrr(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_eqir(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_eqri(REG_TYPE * const regs, struct Instruction const * const instr);

static void
op_eqrr(REG_TYPE * const regs, struct Instruction const * const instr);

//static void
//print_op(enum Op const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Example examples[1024] = {0};
	size_t n_examples = 0;
	size_t n_blanks = 0;
	struct Instruction instructions[4096] = {0};
	size_t n_instructions = 0;
	bool in_program = false;
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
				in_program = true;
			}
			continue;
		}
		n_blanks = 0;

		if (in_program) {
			instructions[n_instructions].op = atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			instructions[n_instructions].a = (REG_TYPE) atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			instructions[n_instructions].b = (REG_TYPE) atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			while (isspace(*ptr)) {
				ptr++;
			}
			instructions[n_instructions].c = (REG_TYPE) atoi(ptr);
			n_instructions++;
			continue;
		}

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

	if (false) {
		for (size_t i = 0; i < n_instructions; i++) {
			printf("%d %d %d %d\n",
					instructions[i].op, instructions[i].a,
					instructions[i].b, instructions[i].c);
		}
	}

	struct Function {
		void (*fn)(REG_TYPE * const, struct Instruction const * const);
		char const * name;
		enum Op op;
	};
	struct Function functions[] = {
		{ .fn = op_addr, .name = "addr", .op = ADDR },
		{ .fn = op_addi, .name = "addi", .op = ADDI },
		{ .fn = op_mulr, .name = "mulr", .op = MULR },
		{ .fn = op_muli, .name = "muli", .op = MULI },
		{ .fn = op_banr, .name = "banr", .op = BANR },
		{ .fn = op_bani, .name = "bani", .op = BANI },
		{ .fn = op_borr, .name = "borr", .op = BORR },
		{ .fn = op_bori, .name = "bori", .op = BORI },
		{ .fn = op_setr, .name = "setr", .op = SETR },
		{ .fn = op_seti, .name = "seti", .op = SETI },
		{ .fn = op_gtir, .name = "gtir", .op = GTIR },
		{ .fn = op_gtri, .name = "gtri", .op = GTRI },
		{ .fn = op_gtrr, .name = "gtrr", .op = GTRR },
		{ .fn = op_eqir, .name = "eqir", .op = EQIR },
		{ .fn = op_eqri, .name = "eqri", .op = EQRI },
		{ .fn = op_eqrr, .name = "eqrr", .op = EQRR },
	};

	enum Op known_opcodes[16] = {0};
	void (*opcode_to_function[16])(REG_TYPE * const, struct Instruction const * const) = {0};
	size_t n_known_opcodes = 0;
	while (n_known_opcodes != 16) {
		for (size_t i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
			struct Function const function = functions[i];

			int count = 0;
			int opcode = -1;
			for (int j = 0; j < 16; j++) {
				if (known_opcodes[j]) {
					continue;
				}

				if (op_satisfies_examples(examples, n_examples, j, function.fn)) {
					count++;
					opcode = j;
				}
			}

			if (count == 1) {
				printf("%d is %s\n", opcode, function.name);
				known_opcodes[opcode] = function.op;
				opcode_to_function[opcode] = function.fn;
				n_known_opcodes++;
			}
		}
	}

	REG_TYPE regs[REG_SZ] = {0};
	for (size_t i = 0; i < n_instructions; i++) {
		struct Instruction const * const instr = &instructions[i];
		opcode_to_function[instr->op](regs, instr);
	}

	printf("%d\n", regs[0]);
	return 0;
}

static bool
op_satisfies_examples(struct Example const * const examples,
		size_t const n_examples,
		int const opcode,
		void (*fn)(REG_TYPE * const, struct Instruction const * const))
{
	for (size_t i = 0; i < n_examples; i++) {
		struct Example const * const example = &examples[i];
		struct Instruction const * const instr = &example->instruction;
		if (instr->op != opcode) {
			continue;
		}
		REG_TYPE regs[REG_SZ] = {0};
		memcpy(regs, example->before, REG_SZ*sizeof(REG_TYPE));
		fn(regs, instr);
		if (memcmp(regs, example->after, REG_SZ*sizeof(REG_TYPE)) == 0) {
			continue;
		}
		return false;
	}

	return true;
}

static void
op_addr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (REG_TYPE) (regs[instr->a]+regs[instr->b]);
}

static void
op_addi(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (REG_TYPE) (regs[instr->a]+instr->b);
}

static void
op_mulr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (REG_TYPE) (regs[instr->a]*regs[instr->b]);
}

static void
op_muli(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (REG_TYPE) (regs[instr->a]*instr->b);
}

static void
op_banr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a]&regs[instr->b];
}

static void
op_bani(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a]&instr->b;
}

static void
op_borr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a]|regs[instr->b];
}

static void
op_bori(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a]|instr->b;
}

static void
op_setr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a];
}

static void
op_seti(REG_TYPE * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = instr->a;
}

static void
op_gtir(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (instr->a > regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_gtri(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] > instr->b) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_gtrr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] > regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqir(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (instr->a == regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqri(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] == instr->b) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqrr(REG_TYPE * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] == regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

//static void
//print_op(enum Op const op)
//{
//	switch (op) {
//	case UNKNOWN:
//		printf("unknown\n");
//		break;
//	case ADDR:
//		printf("addr\n");
//		break;
//	case ADDI:
//		printf("addi\n");
//		break;
//	case MULR:
//		printf("mulr\n");
//		break;
//	case MULI:
//		printf("muli\n");
//		break;
//	case BANR:
//		printf("banr\n");
//		break;
//	case BANI:
//		printf("bani\n");
//		break;
//	case BORR:
//		printf("borr\n");
//		break;
//	case BORI:
//		printf("bori\n");
//		break;
//	case SETR:
//		printf("setr\n");
//		break;
//	case SETI:
//		printf("seti\n");
//		break;
//	case GTIR:
//		printf("gtir\n");
//		break;
//	case GTRI:
//		printf("gtri\n");
//		break;
//	case GTRR:
//		printf("gtrr\n");
//		break;
//	case EQIR:
//		printf("eqir\n");
//		break;
//	case EQRI:
//		printf("eqri\n");
//		break;
//	case EQRR:
//		printf("eqrr\n");
//		break;
//		break;
//	default:
//		assert(1 == 0);
//	}
//}
