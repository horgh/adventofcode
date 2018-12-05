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
	while (1) {
		len = strlen(buf);
		reduce(buf);
		if (strlen(buf) == len) {
			break;
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
