#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 1024

#define I_UNKNOWN 0
#define I_SND     1
#define I_SET     2
#define I_ADD     3
#define I_MUL     4
#define I_MOD     5
#define I_RCV     6
#define I_JGZ     7
#define I_JNZ     8
#define I_SUB     9

#define VALUE    0
#define REGISTER 1

#define DEBUG 0

struct instruction {
	int type;
	int param_type0;
	int param_type1;
	int64_t value0;
	int64_t value1;
	char register0;
	char register1;
};

struct program {
	int id;
	struct htable * registers;
	int instr_idx;
	struct Queue * sendq;
	bool running;
	uint64_t sends;
};

static struct instruction *
parse_instruction(char const * const);
static void
destroy_instructions(struct instruction * * const);
static void
print_instructions(struct instruction * * const);
static void
print_instruction(struct instruction const * const);
static int
run_instruction(int const,
		struct instruction * * const,
		size_t const,
		struct htable * const,
		struct program * const);
static int64_t
register_get(struct htable const * const,
		char const);
static void
register_set(struct htable * const,
		char const,
		int64_t const,
		struct program const * const);
static struct program *
program_create(int64_t const);
static void
program_destroy(struct program * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;
	char buf[4096] = {0};

	struct instruction * instructions[MAX_INSTRUCTIONS] = {0};
	size_t num_instructions = 0;

	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				return 1;
			}
			break;
		}

		instructions[num_instructions] = parse_instruction(buf);
		if (!instructions[num_instructions]) {
			fprintf(stderr, "parse_instruction()\n");
			destroy_instructions(instructions);
			return 1;
		}

		num_instructions++;
	}

	if (0) {
		print_instructions(instructions);
	}

	struct program * const p0 = program_create(0);
	if (!p0) {
		fprintf(stderr, "program_create()\n");
		destroy_instructions(instructions);
		return 1;
	}

	int count = 0;
	while (1) {
		if (instructions[p0->instr_idx]->type == I_MUL) {
			count++;
		}

		p0->instr_idx = run_instruction(p0->instr_idx, instructions,
				num_instructions, p0->registers, p0);

		if (p0->instr_idx < 0 || p0->instr_idx >= (int) num_instructions) {
			break;
		}
	}

	destroy_instructions(instructions);
	program_destroy(p0);

	printf("%d\n", count);
	return 0;
}

static struct instruction *
parse_instruction(char const * const s)
{
	struct instruction * const instr = calloc(1, sizeof(struct instruction));
	if (!instr) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = s;

	if (strncmp(ptr, "snd", 3) == 0) {
		instr->type = I_SND;
	}
	if (strncmp(ptr, "set", 3) == 0) {
		instr->type = I_SET;
	}
	if (strncmp(ptr, "add", 3) == 0) {
		instr->type = I_ADD;
	}
	if (strncmp(ptr, "mul", 3) == 0) {
		instr->type = I_MUL;
	}
	if (strncmp(ptr, "mod", 3) == 0) {
		instr->type = I_MOD;
	}
	if (strncmp(ptr, "rcv", 3) == 0) {
		instr->type = I_RCV;
	}
	if (strncmp(ptr, "jgz", 3) == 0) {
		instr->type = I_JGZ;
	}
	if (strncmp(ptr, "jnz", 3) == 0) {
		instr->type = I_JNZ;
	}
	if (strncmp(ptr, "sub", 3) == 0) {
		instr->type = I_SUB;
	}
	if (instr->type == I_UNKNOWN) {
		fprintf(stderr, "unknown instruction: %s\n", s);
		free(instr);
		return NULL;
	}
	ptr += 4;

	if (instr->type == I_SND) {
		if (isalpha(*ptr)) {
			instr->param_type0 = REGISTER;
			instr->register0 = *ptr;
		} else {
			instr->param_type0 = VALUE;
			instr->value0 = (int64_t) atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_SET || instr->type == I_ADD || instr->type == I_MUL ||
			instr->type == I_MOD || instr->type == I_SUB) {
		instr->param_type0 = REGISTER;
		instr->register0 = *ptr;
		ptr += 2;
		if (isalpha(*ptr)) {
			instr->param_type1 = REGISTER;
			instr->register1 = *ptr;
		} else {
			instr->param_type1 = VALUE;
			instr->value1 = (int64_t) atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_RCV) {
		if (isalpha(*ptr)) {
			instr->param_type0 = REGISTER;
			instr->register0 = *ptr;
		} else {
			instr->param_type0 = VALUE;
			instr->value0 = (int64_t) atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_JGZ || instr->type == I_JNZ) {
		if (isalpha(*ptr)) {
			instr->param_type0 = REGISTER;
			instr->register0 = *ptr;
			ptr += 2;
		} else {
			instr->param_type0 = VALUE;
			instr->value0 = (int64_t) atoll(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			if (isspace(*ptr)) {
				ptr++;
			}
		}
		if (isalpha(*ptr)) {
			instr->param_type1 = REGISTER;
			instr->register1 = *ptr;
		} else {
			instr->param_type1 = VALUE;
			instr->value1 = (int64_t) atoll(ptr);
		}
		return instr;
	}

	fprintf(stderr, "unknown instruction: %s\n", s);
	free(instr);
	return NULL;
}

static void
destroy_instructions(struct instruction * * const instructions)
{
	if (!instructions) {
		return;
	}

	for (size_t i = 0; i < MAX_INSTRUCTIONS; i++) {
		struct instruction * const instr = instructions[i];
		if (!instr) {
			break;
		}
		free(instr);
	}
}

static void
print_instructions(struct instruction * * const instructions)
{
	if (!instructions) {
		return;
	}

	for (size_t i = 0; i < MAX_INSTRUCTIONS; i++) {
		struct instruction * const instr = instructions[i];
		if (!instr) {
			break;
		}

		print_instruction(instr);
	}
}

static void
print_instruction(struct instruction const * const instr)
{
	switch (instr->type) {
	case I_UNKNOWN:
		printf("unknown ");
		break;
	case I_SND:
		printf("snd ");
		break;
	case I_SET:
		printf("set ");
		break;
	case I_ADD:
		printf("add ");
		break;
	case I_MUL:
		printf("mul ");
		break;
	case I_MOD:
		printf("mod ");
		break;
	case I_RCV:
		printf("rcv ");
		break;
	case I_JGZ:
		printf("jgz ");
		break;
	case I_JNZ:
		printf("jnz ");
		break;
	case I_SUB:
		printf("sub ");
		break;
	default:
		printf("very unknown ");
	}

	switch (instr->param_type0) {
	case VALUE:
		printf("%" PRId64 " ", instr->value0);
		break;
	case REGISTER:
		printf("%c ", instr->register0);
		break;
	default:
		printf("unknown type ");
		break;
	}

	if (instr->type == I_SND || instr->type == I_RCV) {
		printf("\n");
		return;
	}

	switch (instr->param_type1) {
	case VALUE:
		printf("%" PRId64 " ", instr->value1);
		break;
	case REGISTER:
		printf("%c", instr->register1);
		break;
	default:
		printf("unknown type");
		break;
	}

	printf("\n");
}

static int
run_instruction(int const instr_idx,
		struct instruction * * const instructions,
		size_t const num_instructions,
		struct htable * const registers,
		struct program * const p)
{
	struct instruction const * const instr = instructions[instr_idx];

	if (DEBUG) {
		printf("program %d running instruction ", p->id);
		print_instruction(instr);
	}

	if (instr->type == I_SND) {
		fprintf(stderr, "unexpectedly saw SND\n");
		return instr_idx+1;
	}

	if (instr->type == I_SET) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0, instr->value1, p);
			return instr_idx+1;
		}
		register_set(registers, instr->register0,
				register_get(registers, instr->register1), p);
		return instr_idx+1;
	}

	if (instr->type == I_ADD) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0,
					register_get(registers, instr->register0)+instr->value1, p);
			return instr_idx+1;
		}
		register_set(registers, instr->register0,
				register_get(registers, instr->register0)+
				register_get(registers, instr->register1), p);
		return instr_idx+1;
	}

	if (instr->type == I_MUL) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0,
					register_get(registers, instr->register0)*instr->value1, p);
			return instr_idx+1;
		}
		register_set(registers, instr->register0,
				register_get(registers, instr->register0)*
				register_get(registers, instr->register1), p);
		return instr_idx+1;
	}

	if (instr->type == I_MOD) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0,
					register_get(registers, instr->register0)%instr->value1, p);
			return instr_idx+1;
		}
		register_set(registers, instr->register0,
				register_get(registers, instr->register0)%
				register_get(registers, instr->register1), p);
		return instr_idx+1;
	}

	if (instr->type == I_RCV) {
		// The parameter is always a register.

		fprintf(stderr, "unexpectedly saw RCV\n");
		return instr_idx+1;
	}

	if (instr->type == I_JGZ) {
		if (instr->param_type0 == VALUE) {
			if (instr->value0 <= 0) {
				return instr_idx+1;
			}
		} else {
			if (register_get(registers, instr->register0) <= 0) {
				return instr_idx+1;
			}
		}

		if (instr->param_type1 == VALUE) {
			return instr_idx+(int) instr->value1;
		}

		int64_t const v = register_get(registers, instr->register1);
		if (v < 0) {
			return -1;
		}
		if (v >= (int64_t) num_instructions) {
			return -1;
		}
		return instr_idx+(int) v;
	}

	if (instr->type == I_JNZ) {
		if (instr->param_type0 == VALUE) {
			if (instr->value0 == 0) {
				return instr_idx+1;
			}
		} else {
			if (register_get(registers, instr->register0) == 0) {
				return instr_idx+1;
			}
		}

		if (instr->param_type1 == VALUE) {
			return instr_idx+(int) instr->value1;
		}

		int64_t const v = register_get(registers, instr->register1);
		if (v < 0) {
			return -1;
		}
		if (v >= (int64_t) num_instructions) {
			return -1;
		}
		return instr_idx+(int) v;
	}

	if (instr->type == I_SUB) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0,
					register_get(registers, instr->register0)-
					instr->value1, p);
			return instr_idx+1;
		}
		register_set(registers, instr->register0,
				register_get(registers, instr->register0)-
				register_get(registers, instr->register1), p);
		return instr_idx+1;
	}

	fprintf(stderr, "unrecognized instruction\n");
	return -1;
}

static int64_t
register_get(struct htable const * const registers,
		char const reg)
{
	char k[2] = {0};
	k[0] = reg;
	int64_t * const v = hash_get(registers, k);
	if (!v) {
		return 0;
	}

	return *v;
}

static void
register_set(struct htable * const registers,
		char const reg,
		int64_t const value,
		struct program const * const p)
{
	char k[2] = {0};
	k[0] = reg;

	if (hash_has_key(registers, k)) {
		if (!hash_delete(registers, k, free)) {
			fprintf(stderr, "hash_delete(%c) failed\n", reg);
			return;
		}
	}

	int64_t * const v = calloc(1, sizeof(int64_t));
	if (!v) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return;
	}

	*v = value;

	if (!hash_set(registers, k, v)) {
		fprintf(stderr, "hash_set(%c, %" PRId64 ") failed\n", reg, *v);
		return;
	}

	if (DEBUG) {
		printf("program %d: register %c = %" PRId64 "\n", p->id, reg, *v);
	}
}

static struct program *
program_create(int64_t const id)
{
	struct program * const p = calloc(1, sizeof(struct program));
	if (!p) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	p->id = (int) id;

	p->registers = hash_init(1024);
	if (!p->registers) {
		fprintf(stderr, "hash_init()\n");
		program_destroy(p);
		return NULL;
	}

	register_set(p->registers, 'p', id, p);

	p->running = true;

	p->sendq = calloc(1, sizeof(struct Queue));
	if (!p->sendq) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		program_destroy(p);
		return NULL;
	}

	return p;
}

static void
program_destroy(struct program * const p)
{
	if (!p) {
		return;
	}

	if (p->registers) {
		if (!hash_free(p->registers, free)) {
			fprintf(stderr, "hash_free(registers)\n");
		}
	}

	if (p->sendq) {
		destroy_queue(p->sendq, free);
	}

	free(p);
}
