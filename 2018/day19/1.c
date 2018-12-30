#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Op {
	ADDR,
	ADDI,
	MULR,
	MULI,
	BANR,
	BANI,
	BORR,
	BORI,
	SETR,
	SETI,
	GTIR,
	GTRI,
	GTRR,
	EQIR,
	EQRI,
	EQRR
};

struct Instruction {
	int op;
	int a;
	int b;
	int c;
};

static void print_instruction(struct Instruction const * const);

static void op_addr(int * const regs, struct Instruction const * const);

static void op_addi(int * const regs, struct Instruction const * const);

static void op_mulr(int * const regs, struct Instruction const * const);

static void op_muli(int * const regs, struct Instruction const * const);

static void op_banr(int * const regs, struct Instruction const * const);

static void op_bani(int * const regs, struct Instruction const * const);

static void op_borr(int * const regs, struct Instruction const * const);

static void op_bori(int * const regs, struct Instruction const * const);

static void op_setr(int * const regs, struct Instruction const * const);

static void op_seti(int * const regs, struct Instruction const * const);

static void op_gtir(int * const regs, struct Instruction const * const);

static void op_gtri(int * const regs, struct Instruction const * const);

static void op_gtrr(int * const regs, struct Instruction const * const);

static void op_eqir(int * const regs, struct Instruction const * const);

static void op_eqri(int * const regs, struct Instruction const * const);

static void op_eqrr(int * const regs, struct Instruction const * const);

#define REGISTERS_SZ 6

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Instruction instructions[128] = {0};
	size_t n_instructions = 0;
	int ip_register = -1;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		if (strncmp(ptr, "#ip ", strlen("#ip ")) == 0) {
			ptr += strlen("#ip ");
			ip_register = atoi(ptr);
			continue;
		}
		if (strncmp(ptr, "addr ", 5) == 0) {
			instructions[n_instructions].op = ADDR;
		} else if (strncmp(ptr, "addi ", 5) == 0) {
			instructions[n_instructions].op = ADDI;
		} else if (strncmp(ptr, "mulr ", 5) == 0) {
			instructions[n_instructions].op = MULR;
		} else if (strncmp(ptr, "muli ", 5) == 0) {
			instructions[n_instructions].op = MULI;
		} else if (strncmp(ptr, "banr ", 5) == 0) {
			instructions[n_instructions].op = BANR;
		} else if (strncmp(ptr, "bani ", 5) == 0) {
			instructions[n_instructions].op = BANI;
		} else if (strncmp(ptr, "borr ", 5) == 0) {
			instructions[n_instructions].op = BORR;
		} else if (strncmp(ptr, "bori ", 5) == 0) {
			instructions[n_instructions].op = BORI;
		} else if (strncmp(ptr, "setr ", 5) == 0) {
			instructions[n_instructions].op = SETR;
		} else if (strncmp(ptr, "seti ", 5) == 0) {
			instructions[n_instructions].op = SETI;
		} else if (strncmp(ptr, "gtir ", 5) == 0) {
			instructions[n_instructions].op = GTIR;
		} else if (strncmp(ptr, "gtri ", 5) == 0) {
			instructions[n_instructions].op = GTRI;
		} else if (strncmp(ptr, "gtrr ", 5) == 0) {
			instructions[n_instructions].op = GTRR;
		} else if (strncmp(ptr, "eqir ", 5) == 0) {
			instructions[n_instructions].op = EQIR;
		} else if (strncmp(ptr, "eqri ", 5) == 0) {
			instructions[n_instructions].op = EQRI;
		} else if (strncmp(ptr, "eqrr ", 5) == 0) {
			instructions[n_instructions].op = EQRR;
		} else {
			assert(1 == 0);
		}
		ptr += 5;
		instructions[n_instructions].a = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		instructions[n_instructions].b = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		instructions[n_instructions].c = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		assert(*ptr == '\0');
		n_instructions++;
	}

	if (0) {
		printf("#ip %d\n", ip_register);
		for (size_t i = 0; i < n_instructions; i++) {
			print_instruction(&instructions[i]);
		}
	}

	int ip = 0;
	int regs[REGISTERS_SZ] = {0};
	while (1) {
		if (ip >= (int)n_instructions) {
			break;
		}

		regs[ip_register] = ip;

		struct Instruction const * const instr = &instructions[ip];
		switch (instr->op) {
		case ADDR:
			op_addr(regs, instr);
			break;
		case ADDI:
			op_addi(regs, instr);
			break;
		case MULR:
			op_mulr(regs, instr);
			break;
		case MULI:
			op_muli(regs, instr);
			break;
		case BANR:
			op_banr(regs, instr);
			break;
		case BANI:
			op_bani(regs, instr);
			break;
		case BORR:
			op_borr(regs, instr);
			break;
		case BORI:
			op_bori(regs, instr);
			break;
		case SETR:
			op_setr(regs, instr);
			break;
		case SETI:
			op_seti(regs, instr);
			break;
		case GTIR:
			op_gtir(regs, instr);
			break;
		case GTRI:
			op_gtri(regs, instr);
			break;
		case GTRR:
			op_gtrr(regs, instr);
			break;
		case EQIR:
			op_eqir(regs, instr);
			break;
		case EQRI:
			op_eqri(regs, instr);
			break;
		case EQRR:
			op_eqrr(regs, instr);
			break;
		default:
			assert(1 == 0);
			break;
		}

		ip = regs[ip_register];
		ip++;
	}

	if (0) {
		for (size_t i = 0; i < REGISTERS_SZ; i++) {
			printf("%d ", regs[i]);
		}
		printf("\n");
	}

	printf("%d\n", regs[0]);
	return 0;
}

static void
print_instruction(struct Instruction const * const instr)
{
	switch (instr->op) {
	case ADDR:
		printf("addr");
		break;
	case ADDI:
		printf("addi");
		break;
	case MULR:
		printf("mulr");
		break;
	case MULI:
		printf("muli");
		break;
	case BANR:
		printf("banr");
		break;
	case BANI:
		printf("bani");
		break;
	case BORR:
		printf("borr");
		break;
	case BORI:
		printf("bori");
		break;
	case SETR:
		printf("setr");
		break;
	case SETI:
		printf("seti");
		break;
	case GTIR:
		printf("gtir");
		break;
	case GTRI:
		printf("gtri");
		break;
	case GTRR:
		printf("gtrr");
		break;
	case EQIR:
		printf("eqir");
		break;
	case EQRI:
		printf("eqri");
		break;
	case EQRR:
		printf("eqrr");
		break;
	default:
		assert(1 == 0);
	}
	printf(" %d %d %d\n", instr->a, instr->b, instr->c);
}

static void
op_addr(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (int)(regs[instr->a] + regs[instr->b]);
}

static void
op_addi(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (int)(regs[instr->a] + instr->b);
}

static void
op_mulr(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (int)(regs[instr->a] * regs[instr->b]);
}

static void
op_muli(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = (int)(regs[instr->a] * instr->b);
}

static void
op_banr(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a] & regs[instr->b];
}

static void
op_bani(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a] & instr->b;
}

static void
op_borr(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a] | regs[instr->b];
}

static void
op_bori(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a] | instr->b;
}

static void
op_setr(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = regs[instr->a];
}

static void
op_seti(int * const regs, struct Instruction const * const instr)
{
	regs[instr->c] = instr->a;
}

static void
op_gtir(int * const regs, struct Instruction const * const instr)
{
	if (instr->a > regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_gtri(int * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] > instr->b) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_gtrr(int * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] > regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqir(int * const regs, struct Instruction const * const instr)
{
	if (instr->a == regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqri(int * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] == instr->b) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}

static void
op_eqrr(int * const regs, struct Instruction const * const instr)
{
	if (regs[instr->a] == regs[instr->b]) {
		regs[instr->c] = 1;
	} else {
		regs[instr->c] = 0;
	}
}
