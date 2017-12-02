#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char const *
get_num(char const * const, int * const);

int main(const int argc, const char * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	size_t const sz = 10240;
	char buf[sz];
	memset(buf, 0, sz);

	int sum = 0;
	while (1) {
		if (fgets(buf, (int) sz, fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				return 1;
			}
			break;
		}

		int lo = -1;
		int hi = -1;
		char const * ptr = buf;
		while (1) {
			int n = -1;
			ptr = get_num(ptr, &n);
			if (!ptr) {
				break;
			}

			if (lo == -1) {
				lo = n;
			}
			if (hi == -1) {
				hi = n;
			}

			if (n < lo) {
				lo = n;
			}
			if (n > hi) {
				hi = n;
			}
		}

		sum += hi-lo;
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
