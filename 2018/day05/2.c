#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

static void
reduce(char * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char buf[409600] = {0};
	assert(fgets(buf, 409600, stdin) != NULL);
	trim_right(buf);

	size_t len = 0;
	for (char c = 'A'; c < 'Z'; c++) {
		char buf2[409600] = {0};
		char const * ptr = buf;
		size_t i = 0;
		while (*ptr != '\0') {
			if (*ptr == c || *ptr - 32 == c) {
				ptr++;
				continue;
			}
			buf2[i++] = *ptr;
			ptr++;
		}

		while (1) {
			size_t const len2 = strlen(buf2);
			reduce(buf2);
			if (strlen(buf2) == len2) {
				break;
			}
		}

		size_t const len2 = strlen(buf2);
		if (len == 0 || len2 < len) {
			len = len2;
		}
	}

	printf("%zu\n", len);
	return 0;
}

static void
reduce(char * const s)
{
	char * ptr = s;
	while (*ptr != '\0' && *(ptr+1) != '\0') {
		if (*ptr - 32 == *(ptr+1) ||
				*ptr + 32 == *(ptr+1)) {
			memcpy(ptr, ptr+2, strlen(ptr+2)+1);
			continue;
		}
		ptr++;
	}
}
