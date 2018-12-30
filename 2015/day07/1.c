#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Operation { Signal, And, Or, Lshift, Rshift, Not };

enum InputType { InSignal, InRegister };

struct Instruction {
	enum Operation operation;
	enum InputType lhs;
	uint16_t in_signal;
	char in_register_0[8];
	char in_register_1[8];
	char out_register[8];
};

static void run_instruction(
		struct Instruction const * const, struct htable * const);

static void print_instruction(struct Instruction const * const);

static void copy_register(char * const, char const ** const);

static void set_register(
		struct htable * const, char const * const, uint16_t const);

static uint16_t get_register(struct htable * const, char const * const);

int
main(int const argc, char const * const * const argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <register> [print]\n", argv[0]);
		return 1;
	}
	char const * const interesting_register = argv[1];
	bool debug = false;
	if (argc == 3) {
		debug = true;
	}

	struct Instruction instructions[1024] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;

		if (strncmp(ptr, "NOT ", strlen("NOT ")) == 0) {
			instructions[n].operation = Not;
			ptr += strlen("NOT ");
			copy_register(instructions[n].in_register_0, &ptr);
			instructions[n].lhs = InRegister;
			ptr += strlen(" -> ");
			copy_register(instructions[n].out_register, &ptr);
			n++;
			continue;
		}

		if (isdigit(*ptr)) {
			instructions[n].in_signal = (uint16_t)atoi(ptr);
			instructions[n].lhs = InSignal;
			while (isdigit(*ptr)) {
				ptr++;
			}
			ptr++;
		} else {
			copy_register(instructions[n].in_register_0, &ptr);
			instructions[n].lhs = InRegister;
			ptr++;
		}

		if (strncmp(ptr, "AND ", strlen("AND ")) == 0) {
			instructions[n].operation = And;
			ptr += strlen("AND ");
			copy_register(instructions[n].in_register_1, &ptr);
		} else if (strncmp(ptr, "OR ", strlen("OR ")) == 0) {
			instructions[n].operation = Or;
			ptr += strlen("OR ");
			copy_register(instructions[n].in_register_1, &ptr);
		} else if (strncmp(ptr, "LSHIFT ", strlen("LSHIFT ")) == 0) {
			instructions[n].operation = Lshift;
			ptr += strlen("LSHIFT ");
			instructions[n].in_signal = (uint16_t)atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
		} else if (strncmp(ptr, "RSHIFT ", strlen("RSHIFT ")) == 0) {
			instructions[n].operation = Rshift;
			ptr += strlen("RSHIFT ");
			instructions[n].in_signal = (uint16_t)atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
		} else if (strncmp(ptr, "-> ", strlen("-> ")) == 0) {
			instructions[n].operation = Signal;
			ptr += strlen("-> ");
			copy_register(instructions[n].out_register, &ptr);
			n++;
			continue;
		} else {
			printf("line %zu: parse failure: [%s]\n", n + 1, ptr);
			return 1;
		}

		ptr += strlen(" -> ");
		copy_register(instructions[n].out_register, &ptr);
		n++;
	}

	struct htable * const registers = hash_init(1024);
	assert(registers != NULL);

	while (1) {
		for (size_t i = 0; i < n; i++) {
			struct Instruction const * const instr = &instructions[i];
			if (debug) {
				print_instruction(instr);
			}
			run_instruction(instr, registers);
		}
		if (hash_has_key(registers, interesting_register)) {
			break;
		}
	}

	uint16_t const interesting_signal =
			get_register(registers, interesting_register);
	assert(hash_free(registers, free));
	printf("%d\n", interesting_signal);
	return 0;
}

static void
run_instruction(
		struct Instruction const * const instr, struct htable * const registers)
{
	if (instr->operation == Signal) {
		if (instr->lhs == InSignal) {
			set_register(registers, instr->out_register, instr->in_signal);
			return;
		}
		if (!hash_has_key(registers, instr->in_register_0)) {
			return;
		}
		uint16_t const a = get_register(registers, instr->in_register_0);
		set_register(registers, instr->out_register, a);
		return;
	}

	if (instr->operation == And) {
		uint16_t a = 0;
		if (instr->lhs == InSignal) {
			a = instr->in_signal;
		} else {
			if (!hash_has_key(registers, instr->in_register_0)) {
				return;
			}
			a = get_register(registers, instr->in_register_0);
		}
		if (!hash_has_key(registers, instr->in_register_1)) {
			return;
		}
		uint16_t const b = get_register(registers, instr->in_register_1);
		set_register(registers, instr->out_register, a & b);
		return;
	}

	if (instr->operation == Or) {
		uint16_t const a = get_register(registers, instr->in_register_0);
		if (!hash_has_key(registers, instr->in_register_0)) {
			return;
		}
		uint16_t const b = get_register(registers, instr->in_register_1);
		if (!hash_has_key(registers, instr->in_register_1)) {
			return;
		}
		set_register(registers, instr->out_register, a | b);
		return;
	}

	if (instr->operation == Lshift) {
		if (!hash_has_key(registers, instr->in_register_0)) {
			return;
		}
		uint16_t const a = get_register(registers, instr->in_register_0);
		uint16_t const signal = (uint16_t)(a << instr->in_signal);
		set_register(registers, instr->out_register, signal);
		return;
	}

	if (instr->operation == Rshift) {
		if (!hash_has_key(registers, instr->in_register_0)) {
			return;
		}
		uint16_t const a = get_register(registers, instr->in_register_0);
		uint16_t const signal = (uint16_t)(a >> instr->in_signal);
		set_register(registers, instr->out_register, signal);
		return;
	}

	if (instr->operation == Not) {
		if (!hash_has_key(registers, instr->in_register_0)) {
			return;
		}
		uint16_t const a = get_register(registers, instr->in_register_0);
		uint16_t const signal = (uint16_t)~a;
		set_register(registers, instr->out_register, signal);
		return;
	}

	assert(1 == 0);
}

static void
print_instruction(struct Instruction const * const instr)
{
	switch (instr->operation) {
	case Signal:
		if (instr->lhs == InSignal) {
			printf("%d -> %s\n", instr->in_signal, instr->out_register);
		} else {
			printf("%s -> %s\n", instr->in_register_0, instr->out_register);
		}
		break;
	case And:
		if (instr->lhs == InSignal) {
			printf("%d AND %s -> %s\n",
					instr->in_signal,
					instr->in_register_1,
					instr->out_register);
		} else {
			printf("%s AND %s -> %s\n",
					instr->in_register_0,
					instr->in_register_1,
					instr->out_register);
		}
		break;
	case Or:
		printf("%s OR %s -> %s\n",
				instr->in_register_0,
				instr->in_register_1,
				instr->out_register);
		break;
	case Lshift:
		printf("%s LSHIFT %d -> %s\n",
				instr->in_register_0,
				instr->in_signal,
				instr->out_register);
		break;
	case Rshift:
		printf("%s RSHIFT %d -> %s\n",
				instr->in_register_0,
				instr->in_signal,
				instr->out_register);
		break;
	case Not:
		printf("NOT %s -> %s\n", instr->in_register_0, instr->out_register);
		break;
	default:
		assert(1 == 0);
		break;
	}
}

static void
copy_register(char * const dst, char const ** const src)
{
	size_t i = 0;
	while (isalpha(**src)) {
		dst[i++] = **src;
		*src += 1;
	}
}

static void
set_register(struct htable * const registers,
		char const * const reg,
		uint16_t const value)
{
	if (hash_has_key(registers, reg)) {
		assert(hash_delete(registers, reg, free));
	}

	uint16_t * const signal = calloc(1, sizeof(uint16_t));
	assert(signal != NULL);
	*signal = value;

	assert(hash_set(registers, reg, signal));
}

static uint16_t
get_register(struct htable * const h, char const * const reg)
{
	uint16_t const * const a = hash_get(h, reg);
	if (!a) {
		return 0;
	}
	return *a;
}
