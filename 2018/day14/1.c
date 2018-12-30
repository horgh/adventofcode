#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t choose_next_recipe(int const * const, size_t const, size_t const);

#define SZ 1024000

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <recipes to make>\n", argv[0]);
		return 1;
	}
	int const recipes_to_make = atoi(argv[1]);

	int * const recipes = calloc(SZ, sizeof(int));
	assert(recipes != NULL);
	size_t n_recipes = 2;
	recipes[0] = 3;
	recipes[1] = 7;
	size_t elf0 = 0;
	size_t elf1 = 1;

	for (int i = 0; i < recipes_to_make + 10; i++) {
		int const new_recipe = recipes[elf0] + recipes[elf1];
		if (new_recipe >= 10) {
			recipes[n_recipes++] = new_recipe / 10;
			recipes[n_recipes++] = new_recipe % 10;
		} else {
			recipes[n_recipes++] = new_recipe;
		}
		elf0 = choose_next_recipe(recipes, n_recipes, elf0);
		elf1 = choose_next_recipe(recipes, n_recipes, elf1);
	}

	for (size_t i = 0; i < 10; i++) {
		printf("%d", recipes[(size_t)recipes_to_make + i]);
	}
	printf("\n");
	return 0;
}

static size_t
choose_next_recipe(int const * const recipes,
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
