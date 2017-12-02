#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
read_file(FILE * const);

int main(const int argc, const char * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char * const buf = read_file(fh);
	if (!buf) {
		fprintf(stderr, "read_file()\n");
		return 1;
	}

	char * p = buf;
	size_t len = 0;
	while (*p != '\0') {
			if (*p < '0' || *p > '9') {
				break;
			}
			p++;
			len++;
	}

	int sum = 0;
	size_t const step = len/2;
	for (size_t i = 0; i < len; i++) {
		int const n = *(buf+i) - '0';

		size_t j = i+step;
		if (j >= len) {
			j -= len;
		}

		int const m = *(buf+j) - '0';

		if (n == m) {
			sum += n;
		}
	}

	printf("%d\n", sum);

	free(buf);
	return 0;
}

static char *
read_file(FILE * const fh)
{
	size_t sz = 256;
	char * buf = calloc(sz, sizeof(char));
	if (!buf) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	size_t used = 0;
	char * ptr = buf;
	while (!feof(fh)) {
		size_t const n = sz-used-1;
		if (fread(ptr, sizeof(char), n, fh) != n) {
			if (feof(fh)) {
				break;
			}

			fprintf(stderr, "fread(): %s\n", strerror(errno));
			free(buf);
			return NULL;
		}

		size_t const new_sz = sz*2;
		char * const buf2 = realloc(buf, new_sz);
		if (!buf2) {
			fprintf(stderr, "realloc(): %s\n", strerror(errno));
			free(buf);
			return NULL;
		}

		buf = buf2;
		sz = new_sz;
		used += n;

		ptr = buf+used;
		memset(ptr, 0, new_sz-used);
	}

	return buf;
}
