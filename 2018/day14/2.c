#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t choose_next_recipe(
		char const * const, size_t const, size_t const);

#define SZ 102400000

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char * const recipes = calloc(SZ, sizeof(char));
	assert(recipes != NULL);
	size_t n_recipes = 2;
	recipes[0] = 3;
	recipes[1] = 7;
	size_t elf0 = 0;
	size_t elf1 = 1;

	uint64_t total_recipes = 2;
	while (1) {
		char const new_recipe = (char)(recipes[elf0] + recipes[elf1]);
		if (new_recipe >= 10) {
			recipes[n_recipes++] = new_recipe / 10;
			recipes[n_recipes++] = new_recipe % 10;
			total_recipes += 2;
		} else {
			recipes[n_recipes++] = new_recipe;
			total_recipes++;
		}
		elf0 = choose_next_recipe(recipes, n_recipes, elf0);
		elf1 = choose_next_recipe(recipes, n_recipes, elf1);

		if (n_recipes < 6) {
			continue;
		}
		if (recipes[n_recipes - 6] == 4 && recipes[n_recipes - 5] == 3 &&
				recipes[n_recipes - 4] == 0 && recipes[n_recipes - 3] == 9 &&
				recipes[n_recipes - 2] == 7 && recipes[n_recipes - 1] == 1) {
			printf("%" PRIu64 "\n", total_recipes - 6);
			break;
		}
		if (n_recipes < 7) {
			continue;
		}
		if (recipes[n_recipes - 7] == 4 && recipes[n_recipes - 6] == 3 &&
				recipes[n_recipes - 5] == 0 && recipes[n_recipes - 4] == 9 &&
				recipes[n_recipes - 3] == 7 && recipes[n_recipes - 2] == 1) {
			printf("%" PRIu64 "\n", total_recipes - 7);
			break;
		}
	}

	return 0;
}

static size_t
choose_next_recipe(char const * const recipes,
		size_t const n_recipes,
		size_t const current_recipe)
{
	size_t const steps = (size_t)recipes[current_recipe] + 1;
	size_t const new_pos = current_recipe + steps;
	if (new_pos < n_recipes) {
		return new_pos;
	}
	return new_pos % n_recipes;
}
