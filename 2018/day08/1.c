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

	int sum = 0;

	for (int i = 0; i < n_node; i++) {
		char const * s2 = ptr;
		sum += consume_node(s2, &ptr);
	}

	for (int i = 0; i < n_metadata; i++) {
		int const metadata = atoi(ptr);
		sum += metadata;
		while (isdigit(*ptr)) {
			ptr++;
		}
		if (*ptr != '\0') {
			ptr++;
		}
	}

	*after = ptr;
	return sum;
}
