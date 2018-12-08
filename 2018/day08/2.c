#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

static int
consume_node(char const * const, char const * * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char buf[40960] = {0};
	assert(fgets(buf, 40960, stdin) != NULL);
	trim_right(buf);

	char const * ptr = buf;
	int const sum = consume_node(buf, &ptr);

	printf("%d\n", sum);
	return 0;
}

static int
consume_node(char const * const s, char const * * const after)
{
	if (*s == '\0') {
		return 0;
	}

	char const * ptr = s;

	int const n_node = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	ptr++;

	int const n_metadata = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	if (*ptr != '\0') {
		ptr++;
	}

	int node_values[64] = {0};
	assert(n_node < 64);
	for (int i = 0; i < n_node; i++) {
		char const * s2 = ptr;
		int const node_value = consume_node(s2, &ptr);
		node_values[i] = node_value;
	}

	int value = 0;
	int metadata_sum = 0;
	for (int i = 0; i < n_metadata; i++) {
		int const metadata = atoi(ptr);
		metadata_sum += metadata;
		if (metadata > 0) {
			if (metadata <= n_node) {
				value += node_values[metadata-1];
			}
		}
		while (isdigit(*ptr)) {
			ptr++;
		}
		if (*ptr != '\0') {
			ptr++;
		}
	}

	if (n_node == 0) {
		value = metadata_sum;
	}

	*after = ptr;
	return value;
}
