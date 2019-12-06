#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void solve(char const * const);
static bool valid(int const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		solve(buf);
	}

	return 0;
}

static void solve(char const * const buf) {
	char const * ptr = buf;
	int const start = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	if (*ptr == '-') {
		ptr++;
	}
	int const end = atoi(ptr);

	int count = 0;
	for (int i = start; i <= end; i++) {
		if (!valid(i)) {
			continue;
		}
		count++;
	}

	printf("%d\n", count);
}

static bool valid(int const n) {
	int x = 100000;
	bool have_repeat = false;
	int last = -1;
	for (int i = 0; i < 6; i++) {
		int digit = n/x%10;
		x /= 10;

		if (last != -1) {
			if (digit == last) {
				have_repeat = true;
			}
			if (digit < last) {
				return false;
			}
		}
		last = digit;
	}

	return have_repeat;
}
