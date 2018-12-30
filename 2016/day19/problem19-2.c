#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Elf {
	int number;
};

static struct Elf ** __remove_elf(struct Elf ** const, const int, const int);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input>\n", argv[0]);
		return 1;
	}
	const int input = atoi(argv[1]);

	struct Elf ** elves = calloc((size_t)input, sizeof(struct Elf *));

	for (int i = 0; i < input; i++) {
		elves[i] = calloc(1, sizeof(struct Elf));
		if (!elves[i]) {
			printf("%s\n", strerror(errno));
			return 1;
		}
		elves[i]->number = i;
	}

	int num_elves = input;

	while (num_elves > 1) {
		for (int i = 0; i < num_elves; i++) {
			// if (num_elves % 1000 == 0) {
			//	printf("%d...\n", num_elves);
			//}

			// Find index of elf across from us in the circle.
			int index = num_elves / 2 + i;
			if (index >= num_elves) {
				// Start back at 0 if we overshoot.
				index = index - num_elves;
			}

			// printf("i %d num_elves %d index %d\n", i, num_elves, index);
			// printf("elf %d steals from elf %d\n", elves[i]->number+1,
			//		elves[index]->number+1);

			elves = __remove_elf(elves, num_elves, index);

			num_elves--;

			// If we remove an elf below us in index, we need to drop down our current
			// index, else we can fail to visit all elves in the circle when going
			// around. Try with n=6 or n=7 to see.
			if (index < i) {
				i--;
			}
		}
	}

	printf("%d\n", elves[0]->number + 1);

	free(elves[0]);
	free(elves);

	return 0;
}

static struct Elf **
__remove_elf(struct Elf ** const elves, const int num_elves, const int index)
{
	free(elves[index]);
	for (int i = index; i < num_elves - 1; i++) {
		elves[i] = elves[i + 1];
	}

	return elves;
}
