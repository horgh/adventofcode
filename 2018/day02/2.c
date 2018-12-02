#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
compare_ids(char const * const, char const * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char * ids[250] = {0};
	int n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		ids[n] = calloc(128, sizeof(char));
		assert(ids[n] != NULL);
		memcpy(ids[n], buf, strlen(buf)+1);
		n++;
	}

	for (int i = 0; i < n; i++) {
		for (int j = i+1; j < n; j++) {
			int const diff = compare_ids(ids[i], ids[j]);
			if (diff != 1) {
				continue;
			}
			printf("%s and %s\n", ids[i], ids[j]);
			size_t const m = strlen(ids[i]);
			for (size_t k = 0; k < m; k++) {
				if (ids[i][k] == ids[j][k]) {
					printf("%c", ids[i][k]);
				}
			}
			printf("\n");
		}
	}

	return 0;
}

__attribute__((pure))
static int
compare_ids(char const * const a, char const * const b)
{
	size_t n = strlen(a);
	int count = 0;
	for (size_t i = 0; i < n; i++) {
		if (a[i] == b[i]) {
			continue;
		}
		count++;
	}
	return count;
}
