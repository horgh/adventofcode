#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 1024

#define I_UNKNOWN 0
#define I_SND 1
#define I_SET 2
#define I_ADD 3
#define I_MUL 4
#define I_MOD 5
#define I_RCV 6
#define I_JGZ 7

#define VALUE 0
#define REGISTER 1

struct instruction {
	int type;
	int param_type0;
	int param_type1;
	int64_t value0;
	int64_t value1;
	char register0;
	char register1;
};

static struct instruction * parse_instruction(char const * const);
static void destroy_instructions(struct instruction ** const);
static void print_instructions(struct instruction ** const);
static void print_instruction(struct instruction const * const);
static int run_instruction(int const,
		struct instruction ** const,
		size_t const,
		struct htable * const,
		int64_t * const,
		bool * const);
static int64_t register_get(struct htable const * const, char const);
static void register_set(struct htable * const, char const, int64_t const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;
	char buf[4096] = {0};

	struct instruction * instructions[MAX_INSTRUCTIONS] = {0};
	size_t num_instructions = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
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

	struct htable * const registers = hash_init(1024);
	if (!registers) {
		fprintf(stderr, "hash_init()\n");
		destroy_instructions(instructions);
		return 1;
	}

	int instr_idx = 0;
	int64_t frequency = 0;
	while (1) {
		if (0) {
			printf("idx %d: running instruction: ", instr_idx);
			print_instruction(instructions[instr_idx]);
		}

		bool recovered_frequency = false;
		instr_idx = run_instruction(instr_idx,
				instructions,
				num_instructions,
				registers,
				&frequency,
				&recovered_frequency);
		if (recovered_frequency) {
			printf("%" PRId64 "\n", frequency);
			break;
		}
		if (instr_idx < 0 || instr_idx >= (int)num_instructions) {
			break;
		}
	}

	if (!hash_free(registers, free)) {
		fprintf(stderr, "hash_free()\n");
		destroy_instructions(instructions);
		return 1;
	}

	destroy_instructions(instructions);

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
			instr->value0 = (int64_t)atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_SET || instr->type == I_ADD || instr->type == I_MUL ||
			instr->type == I_MOD) {
		instr->param_type0 = REGISTER;
		instr->register0 = *ptr;
		ptr += 2;
		if (isalpha(*ptr)) {
			instr->param_type1 = REGISTER;
			instr->register1 = *ptr;
		} else {
			instr->param_type1 = VALUE;
			instr->value1 = (int64_t)atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_RCV) {
		if (isalpha(*ptr)) {
			instr->param_type0 = REGISTER;
			instr->register0 = *ptr;
		} else {
			instr->param_type0 = VALUE;
			instr->value0 = (int64_t)atoll(ptr);
		}
		return instr;
	}

	if (instr->type == I_JGZ) {
		if (isalpha(*ptr)) {
			instr->param_type0 = REGISTER;
			instr->register0 = *ptr;
			ptr += 2;
		} else {
			instr->param_type0 = VALUE;
			instr->value0 = (int64_t)atoll(ptr);
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
			instr->value1 = (int64_t)atoll(ptr);
		}
		return instr;
	}

	fprintf(stderr, "unknown instruction: %s\n", s);
	free(instr);
	return NULL;
}

static void
destroy_instructions(struct instruction ** const instructions)
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
print_instructions(struct instruction ** const instructions)
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
		struct instruction ** const instructions,
		size_t const num_instructions,
		struct htable * const registers,
		int64_t * const frequency,
		bool * const recovered_frequency)
{
	struct instruction const * const instr = instructions[instr_idx];

	if (instr->type == I_SND) {
		if (instr->param_type0 == VALUE) {
			*frequency = instr->value0;
			return instr_idx + 1;
		}

		*frequency = register_get(registers, instr->register0);
		return instr_idx + 1;
	}

	if (instr->type == I_SET) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers, instr->register0, instr->value1);
			return instr_idx + 1;
		}
		register_set(
				registers, instr->register0, register_get(registers, instr->register1));
		return instr_idx + 1;
	}

	if (instr->type == I_ADD) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers,
					instr->register0,
					register_get(registers, instr->register0) + instr->value1);
			return instr_idx + 1;
		}
		register_set(registers,
				instr->register0,
				register_get(registers, instr->register0) +
						register_get(registers, instr->register1));
		return instr_idx + 1;
	}

	if (instr->type == I_MUL) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers,
					instr->register0,
					register_get(registers, instr->register0) * instr->value1);
			return instr_idx + 1;
		}
		register_set(registers,
				instr->register0,
				register_get(registers, instr->register0) *
						register_get(registers, instr->register1));
		return instr_idx + 1;
	}

	if (instr->type == I_MOD) {
		// Param 0 should always be a register.

		if (instr->param_type1 == VALUE) {
			register_set(registers,
					instr->register0,
					register_get(registers, instr->register0) % instr->value1);
			return instr_idx + 1;
		}
		register_set(registers,
				instr->register0,
				register_get(registers, instr->register0) %
						register_get(registers, instr->register1));
		return instr_idx + 1;
	}

	if (instr->type == I_RCV) {
		// Apparently always a register.
		if (register_get(registers, instr->register0) == 0) {
			return instr_idx + 1;
		}

		*recovered_frequency = true;
		return instr_idx + 1;
	}

	if (instr->type == I_JGZ) {
		if (instr->param_type0 == VALUE) {
			if (instr->value0 <= 0) {
				return instr_idx + 1;
			}
		} else {
			if (register_get(registers, instr->register0) <= 0) {
				return instr_idx + 1;
			}
		}

		if (instr->param_type1 == VALUE) {
			return instr_idx + (int)instr->value1;
		}

		int64_t const v = register_get(registers, instr->register1);
		if (v < 0) {
			return -1;
		}
		if (v >= (int64_t)num_instructions) {
			return -1;
		}
		return instr_idx + (int)v;
	}

	fprintf(stderr, "unrecognized instruction\n");
	return -1;
}

static int64_t
register_get(struct htable const * const registers, char const reg)
{
	char k[2] = {0};
	k[0] = reg;
	int64_t * const v = hash_get(registers, k);
	if (!v) {
		return 0;
	}

	if (0) {
		printf("register %c has value %" PRId64 "\n", reg, *v);
	}
	return *v;
}

static void
register_set(
		struct htable * const registers, char const reg, int64_t const value)
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
}
