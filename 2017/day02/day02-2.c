#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char const * get_num(char const * const, int * const);
static int evenly_divide(char const * const, int const);

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	size_t const sz = 10240;
	char buf[sz];
	memset(buf, 0, sz);

	int sum = 0;
	while (1) {
		if (fgets(buf, (int)sz, fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				return 1;
			}
			break;
		}

		char const * ptr = buf;
		while (1) {
			int n = -1;
			ptr = get_num(ptr, &n);
			if (!ptr) {
				break;
			}

			int const d = evenly_divide(ptr, n);
			if (d != -1) {
				sum += d;
				break;
			}
		}
	}

	printf("%d\n", sum);

	return 0;
}

static char const *
get_num(char const * const s, int * const n)
{
	if (*s == '\0' || *s == '\n') {
		return NULL;
	}

	char const * ptr = s;
	while (isspace(*ptr)) {
		ptr++;
	}

	*n = atoi(ptr);

	while (*ptr >= '0' && *ptr <= '9') {
		ptr++;
	}

	return ptr;
}

static int
evenly_divide(char const * const s, int const n)
{
	char const * ptr = s;
	while (1) {
		int m = -1;
		ptr = get_num(ptr, &m);
		if (!ptr) {
			return -1;
		}

		int numerator = -1;
		int denominator = -1;
		if (n >= m) {
			numerator = n;
			denominator = m;
		} else {
			numerator = m;
			denominator = n;
		}

		if (denominator == 0) {
			continue;
		}

		if (numerator % denominator == 0) {
			return numerator / denominator;
		}
	}

	return -1;
}
