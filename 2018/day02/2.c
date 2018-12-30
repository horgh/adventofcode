#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>

static int compare_ids(char const * const, char const * const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char ids[250][128] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		memcpy(ids[n], buf, strlen(buf) + 1);
		n++;
	}

	for (size_t i = 0; i < n; i++) {
		for (size_t j = i + 1; j < n; j++) {
			if (compare_ids(ids[i], ids[j]) != 1) {
				continue;
			}

			size_t const m = strlen(ids[i]);
			for (size_t k = 0; k < m; k++) {
				if (ids[i][k] != ids[j][k]) {
					continue;
				}
				printf("%c", ids[i][k]);
			}
		}
	}

	return 0;
}

__attribute__((pure)) static int
compare_ids(char const * const a, char const * const b)
{
	size_t const n = strlen(a);
	int count = 0;
	for (size_t i = 0; i < n; i++) {
		if (a[i] == b[i]) {
			continue;
		}
		count++;
	}
	return count;
}
