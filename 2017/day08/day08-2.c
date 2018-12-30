#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INC 0
#define DEC 1
#define GT 2
#define LT 3
#define GE 4
#define EQ 5
#define LE 6
#define NE 7

struct Instruction {
	char * reg;
	int op;
	int val;
	char * cond_reg;
	int cond_op;
	int cond_val;
};

static struct Instruction * parse_line(char const * const);
static void instruction_destroy(struct Instruction * const);
static void instruction_print(struct Instruction * const);
static char const * op_to_string(int const);
static bool cond(struct Instruction const * const, int const);
static int get_largest_value_in_registers(struct htable const * const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	char buf[10240] = {0};

	struct htable * const h = hash_init(1024);
	if (!h) {
		fprintf(stderr, "hash_init()\n");
		return 1;
	}

	int largest = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				hash_free(h, free);
				return 1;
			}
			break;
		}

		struct Instruction * const instr = parse_line(buf);
		if (!instr) {
			fprintf(stderr, "parse_line()\n");
			hash_free(h, free);
			return 1;
		}

		if (0) {
			instruction_print(instr);
		}

		if (!hash_has_key(h, instr->cond_reg)) {
			int * const v = calloc(1, sizeof(int));
			if (!v) {
				fprintf(stderr, "calloc(): %s\n", strerror(errno));
				hash_free(h, free);
				instruction_destroy(instr);
				return 1;
			}

			if (!hash_set(h, instr->cond_reg, v)) {
				fprintf(stderr, "hash_set()\n");
				hash_free(h, free);
				instruction_destroy(instr);
				free(v);
				return 1;
			}
		}

		int * const reg_val = hash_get(h, instr->cond_reg);
		if (!reg_val) {
			fprintf(stderr, "hash_get(cond_reg=%s)\n", instr->cond_reg);
			hash_free(h, free);
			instruction_destroy(instr);
			return 1;
		}

		if (!cond(instr, *reg_val)) {
			instruction_destroy(instr);
			continue;
		}

		if (!hash_has_key(h, instr->reg)) {
			int * const v = calloc(1, sizeof(int));
			if (!v) {
				fprintf(stderr, "calloc(): %s\n", strerror(errno));
				hash_free(h, free);
				instruction_destroy(instr);
				return 1;
			}

			if (!hash_set(h, instr->reg, v)) {
				fprintf(stderr, "hash_set()\n");
				hash_free(h, free);
				instruction_destroy(instr);
				free(v);
				return 1;
			}
		}

		int * const val = hash_get(h, instr->reg);
		if (!val) {
			fprintf(stderr, "hash_get(reg=%s)\n", instr->reg);
			hash_free(h, free);
			instruction_destroy(instr);
			return 1;
		}

		if (instr->op == INC) {
			*val += instr->val;
		} else if (instr->op == DEC) {
			*val -= instr->val;
		} else {
			fprintf(stderr, "unhandled instruction\n");
			hash_free(h, free);
			instruction_destroy(instr);
			return 1;
		}

		int const current_largest = get_largest_value_in_registers(h);
		if (current_largest == -1) {
			fprintf(stderr, "get_largest_value_in_registers()\n");
			hash_free(h, free);
			instruction_destroy(instr);
			return 1;
		}

		if (current_largest > largest) {
			largest = current_largest;
		}

		instruction_destroy(instr);
	}

	if (!hash_free(h, free)) {
		fprintf(stderr, "hash_free()\n");
		return 1;
	}

	printf("%d\n", largest);
	return 0;
}

static struct Instruction *
parse_line(char const * const s)
{
	struct Instruction * const instr = calloc(1, sizeof(struct Instruction));
	if (!instr) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	instr->reg = calloc(strlen(s) + 1, sizeof(char));
	if (!instr->reg) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		instruction_destroy(instr);
		return NULL;
	}

	instr->cond_reg = calloc(strlen(s) + 1, sizeof(char));
	if (!instr->cond_reg) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		instruction_destroy(instr);
		return NULL;
	}

	char const * ptr = s;
	size_t i = 0;
	while (*ptr != ' ') {
		instr->reg[i++] = *ptr;
		ptr++;
	}
	ptr++;

	if (strncmp(ptr, "inc", 3) == 0) {
		instr->op = INC;
	} else if (strncmp(ptr, "dec", 3) == 0) {
		instr->op = DEC;
	} else {
		fprintf(stderr, "unrecognized op\n");
		instruction_destroy(instr);
		return NULL;
	}
	ptr += 3;

	if (*ptr != ' ') {
		fprintf(stderr, "missing space after instruction\n");
		instruction_destroy(instr);
		return NULL;
	}
	ptr++;

	instr->val = atoi(ptr);

	while (*ptr == '-' || isdigit(*ptr) || *ptr == ' ') {
		ptr++;
	}

	if (*ptr != 'i' || *(ptr + 1) != 'f' || *(ptr + 2) != ' ') {
		fprintf(stderr, "missing 'if ', at [%s]\n", ptr);
		instruction_destroy(instr);
		return NULL;
	}
	ptr += 3;

	i = 0;
	while (isalpha(*ptr)) {
		instr->cond_reg[i++] = *ptr;
		ptr++;
	}

	if (*ptr != ' ') {
		fprintf(stderr, "missing space after condition register\n");
		instruction_destroy(instr);
		return NULL;
	}
	ptr++;

	if (strncmp(ptr, ">=", 2) == 0) {
		instr->cond_op = GE;
		ptr += 2;
	} else if (strncmp(ptr, ">", 1) == 0) {
		instr->cond_op = GT;
		ptr += 1;
	} else if (strncmp(ptr, "<=", 2) == 0) {
		instr->cond_op = LE;
		ptr += 2;
	} else if (strncmp(ptr, "<", 1) == 0) {
		instr->cond_op = LT;
		ptr += 1;
	} else if (strncmp(ptr, "!=", 2) == 0) {
		instr->cond_op = NE;
		ptr += 2;
	} else if (strncmp(ptr, "==", 2) == 0) {
		instr->cond_op = EQ;
		ptr += 2;
	} else {
		fprintf(stderr, "invalid op: [%s]\n", ptr);
		instruction_destroy(instr);
		return NULL;
	}

	if (*ptr != ' ') {
		fprintf(stderr, "missing space after cond op\n");
		instruction_destroy(instr);
		return NULL;
	}
	ptr++;

	instr->cond_val = atoi(ptr);

	return instr;
}

static void
instruction_destroy(struct Instruction * const instr)
{
	if (!instr) {
		return;
	}

	if (instr->reg) {
		free(instr->reg);
	}

	if (instr->cond_reg) {
		free(instr->cond_reg);
	}

	free(instr);
}

static void
instruction_print(struct Instruction * const instr)
{
	printf("reg [%s] op [%s] val [%d] cond_reg [%s] cond_op [%s] cond_val [%d]\n",
			instr->reg,
			op_to_string(instr->op),
			instr->val,
			instr->cond_reg,
			op_to_string(instr->cond_op),
			instr->cond_val);
}

static char const *
op_to_string(int const op)
{
	if (op == INC) {
		return "INC";
	}
	if (op == DEC) {
		return "DEC";
	}
	if (op == GT) {
		return "GT";
	}
	if (op == LT) {
		return "LT";
	}
	if (op == GE) {
		return "GE";
	}
	if (op == EQ) {
		return "EQ";
	}
	if (op == LE) {
		return "LE";
	}
	if (op == NE) {
		return "NE";
	}
	return "UNKNOWN";
}

static bool
cond(struct Instruction const * const instr, int const val)
{
	if (instr->cond_op == GT) {
		return val > instr->cond_val;
	}
	if (instr->cond_op == LT) {
		return val < instr->cond_val;
	}
	if (instr->cond_op == GE) {
		return val >= instr->cond_val;
	}
	if (instr->cond_op == EQ) {
		return val == instr->cond_val;
	}
	if (instr->cond_op == LE) {
		return val <= instr->cond_val;
	}
	if (instr->cond_op == NE) {
		return val != instr->cond_val;
	}

	fprintf(stderr,
			"unrecognized op %s, %d\n",
			op_to_string(instr->cond_op),
			instr->cond_op);
	return false;
}

static int
get_largest_value_in_registers(struct htable const * const h)
{
	void ** const keys = hash_get_keys(h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys()\n");
		return -1;
	}

	int largest = 0;
	for (size_t i = 0; keys[i]; i++) {
		int * const val = hash_get(h, keys[i]);
		if (!val) {
			fprintf(stderr, "hash_get(key=%s)\n", (char *)keys[i]);
			hash_free_keys(keys);
			return -1;
		}

		if (*val > largest) {
			largest = *val;
		}
	}

	hash_free_keys(keys);

	return largest;
}
