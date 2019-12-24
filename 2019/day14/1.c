#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Ingredient {
	int amount;
	char name[64];
	size_t n_name;
};

struct Reaction {
	struct Ingredient inputs[16];
	size_t n_inputs;
	struct Ingredient output;
};

static int produce(struct Reaction const * const,
		size_t const,
		struct htable * const,
		int const,
		char const * const,
		int * const);
static int get(struct htable const * const, char const * const);
static void set(struct htable * const,
		char const * const,
		int const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Reaction reactions[64] = {0};
	size_t n_reactions = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}

		struct Reaction reaction = {0};
		char const * ptr = buf;
		while (1) {
			struct Ingredient input = {0};

			input.amount = atoi(ptr);
			while (isdigit(*ptr)) {
				ptr++;
			}
			ptr++;

			while (isalpha(*ptr)) {
				input.name[input.n_name++] = *ptr;
				ptr++;
			}

			reaction.inputs[reaction.n_inputs++] = input;

			if (*ptr == ',') {
				ptr += 2;
				continue;
			}
			ptr += strlen(" => ");
			break;
		}

		struct Ingredient output = {0};
		output.amount = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		while (isalpha(*ptr)) {
			output.name[output.n_name++] = *ptr;
			ptr++;
		}
		reaction.output = output;

		reactions[n_reactions++] = reaction;
	}

	struct htable * const h = hash_init(1024);
	assert(h != NULL);
	int count = 0;
	produce(reactions, n_reactions, h, 1, "FUEL", &count);
	printf("%d\n", count);

	return 0;
}

static int produce(struct Reaction const * const reactions,
		size_t const n_reactions,
		struct htable * const h,
		int const target_amount,
		char const * const target,
		int * const count) {
	struct Reaction const * reaction = NULL;
	for (size_t i = 0; i < n_reactions; i++) {
		if (strcmp(reactions[i].output.name, target) != 0) {
			continue;
		}
		reaction = &reactions[i];
		break;
	}
	assert(reaction != NULL);

	int produced = 0;
	for ( ; produced < target_amount; produced += reaction->output.amount) {
		for (size_t i = 0; i < reaction->n_inputs; i++) {
			int have_amount = get(h, reaction->inputs[i].name);
			if (have_amount < reaction->inputs[i].amount) {
				if (strcmp(reaction->inputs[i].name, "ORE") == 0) {
					*count      += reaction->inputs[i].amount;
					have_amount += reaction->inputs[i].amount;
				} else {
					int const need_amount = reaction->inputs[i].amount-have_amount;
					have_amount += produce(reactions,
							n_reactions,
							h,
							need_amount,
							reaction->inputs[i].name,
							count);
				}
			}

			have_amount -= reaction->inputs[i].amount;
			set(h, reaction->inputs[i].name, have_amount);
		}
	}

	return produced;
}

static int get(struct htable const * const h, char const * const key) {
	int const * const v = hash_get(h, key);
	if (!v) {
		return 0;
	}
	return (int) *v;
}

static void set(struct htable * const h,
		char const * const key,
		int const v) {
	int * const vi = calloc(1, sizeof(int));
	assert(vi != NULL);
	*vi = v;
	assert(hash_set(h, key, vi));
}
