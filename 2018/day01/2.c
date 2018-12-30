#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	int input[4096] = {0};
	int i = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		input[i++] = atoi(buf);
	}

	struct htable * const h = hash_init(1024);
	assert(h != NULL);

	int freq = 0;
	assert(hash_set_i(h, freq, NULL));

	int j = 0;
	while (1) {
		freq += input[j];
		j++;
		if (j == i) {
			j = 0;
		}

		if (hash_has_key_i(h, freq)) {
			break;
		}

		assert(hash_set_i(h, freq, NULL));
	}

	assert(hash_free(h, NULL));

	printf("%d\n", freq);
	return 0;
}
