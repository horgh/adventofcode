#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input>\n", argv[0]);
		return 1;
	}
	const int input = atoi(argv[1]);

	int * const elves = calloc((size_t) input, sizeof(int));

	for (int i = 0; i < input; i++) {
		elves[i] = 1;
	}

	while (1) {
		for (int i = 0; i < input; i++) {
			if (elves[i] == 0) {
				continue;
			}

			int index = -1;

			// To its left until we hit n
			for (int j = i+1; j < input; j++) {
				if (elves[j] > 0) {
					index = j;
					break;
				}
			}

			// If we didn't find any to our left, start over from 0 until we hit
			// ourself.
			if (index == -1) {
				for (int j = 0; j < i; j++) {
					if (elves[j] > 0) {
						index = j;
						break;
					}
				}
			}

			// If no other elves found, we're the winner.
			if (index == -1) {
				printf("%d\n", i+1);
				free(elves);
				return 0;
			}

			elves[i] += elves[index];
			elves[index] = 0;
		}
	}

	free(elves);
	return 0;
}
