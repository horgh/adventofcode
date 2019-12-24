#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <map.h>
#include <math.h>
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

struct Cost {
	int ore;
	int amount;
	struct htable * have;
};

static int produce(struct htable const * const,
		struct htable * const,
		struct htable * const,
		int const,
		char const * const,
		int * const);
static int get(struct htable const * const, char const * const);
static void set(struct htable * const,
		char const * const,
		int const);
static void update(struct htable * const,
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

	struct htable * const reactions_hash = hash_init(1024);
	assert(reactions_hash != NULL);

	for (size_t i = 0; i < n_reactions; i++) {
		assert(hash_set(reactions_hash, reactions[i].output.name, &reactions[i]));
	}

	struct htable * const have = hash_init(1024);
	assert(have != NULL);
	int64_t const ore = 1000000000000;
	int64_t ore_used = 0;
	int64_t fuel = 0;
	while (1) {
		int count = 0;
		produce(reactions_hash, have, NULL, 1, "FUEL", &count);
		ore_used += (int64_t) count;
		if (ore_used >= ore) {
			break;
		}
		fuel++;
		if (fuel % 100 == 0) {
			printf("%" PRId64 ", %.2f%%, %d\n", fuel, (double) ore_used/(double) ore*100, count);
		}
	}
	printf("%" PRId64 "\n", fuel);

	return 0;
}

static int produce(struct htable const * const reactions,
		struct htable * const have,
		struct htable * const used,
		int const target_amount,
		char const * const target,
		int * const count) {
	struct Reaction const * const reaction = hash_get(reactions, target);
	assert(reaction != NULL);

	int produced = 0;
	for ( ; produced < target_amount; produced += reaction->output.amount) {
		for (size_t i = 0; i < reaction->n_inputs; i++) {
			int have_amount = get(have, reaction->inputs[i].name);
			if (have_amount < reaction->inputs[i].amount) {
				if (strcmp(reaction->inputs[i].name, "ORE") == 0) {
					*count      += reaction->inputs[i].amount;
					have_amount += reaction->inputs[i].amount;
				} else {
					int const need_amount = reaction->inputs[i].amount-have_amount;
					have_amount += produce(reactions,
							have,
							used,
							need_amount,
							reaction->inputs[i].name,
							count);
				}
			}

			have_amount -= reaction->inputs[i].amount;
			set(have, reaction->inputs[i].name, have_amount);
			if (used) {
				update(used, reaction->inputs[i].name, reaction->inputs[i].amount);
			}
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
	if (hash_has_key(h, key)) {
		assert(hash_delete(h, key, free));
	}
	int * const vi = calloc(1, sizeof(int));
	assert(vi != NULL);
	*vi = v;
	assert(hash_set(h, key, vi));
}

static void update(struct htable * const h,
		char const * const key,
		int const v) {
	int const cur = get(h, key);
	set(h, key, cur+v);
}
