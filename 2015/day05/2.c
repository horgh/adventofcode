#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

static bool is_nice(char const * const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	int count = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, 8192, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		if (is_nice(buf)) {
			count++;
		}
	}

	printf("%d\n", count);
	return 0;
}

__attribute__((pure)) static bool
is_nice(char const * const s)
{
	bool has_pair = false;
	bool has_same_separated_by_one = false;
	char const * ptr = s;
	while (*ptr != '\0') {
		if (*(ptr + 1) != '\0') {
			char buf[3] = {0};
			buf[0] = *ptr;
			buf[1] = *(ptr + 1);
			if (strstr(ptr + 2, buf) != NULL) {
				has_pair = true;
			}
		}

		if (*(ptr + 1) != '\0' && *(ptr + 2) != '\0') {
			if (*ptr != *(ptr + 1) && *ptr == *(ptr + 2)) {
				has_same_separated_by_one = true;
			}
		}

		ptr++;
	}

	return has_pair && has_same_separated_by_one;
}
