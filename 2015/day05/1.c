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
	int vowel_count = 0;
	bool has_two_in_a_row = false;
	bool has_bad_string = false;
	char const * ptr = s;
	while (*ptr != '\0') {
		if (*ptr == 'a' || *ptr == 'e' || *ptr == 'i' || *ptr == 'o' ||
				*ptr == 'u') {
			vowel_count++;
		}

		char const next_char = *(ptr + 1);

		if (*ptr == next_char) {
			has_two_in_a_row = true;
		}

		if ((*ptr == 'a' && next_char == 'b') ||
				(*ptr == 'c' && next_char == 'd') ||
				(*ptr == 'p' && next_char == 'q') ||
				(*ptr == 'x' && next_char == 'y')) {
			has_bad_string = true;
		}
		ptr++;
	}

	return vowel_count >= 3 && has_two_in_a_row && !has_bad_string;
}
