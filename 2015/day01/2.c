#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char buf[8192] = {0};
	assert(fgets(buf, 8192, stdin) != NULL);

	char const * ptr = buf;
	int floor = 0;
	int i = 0;
	while (*ptr == '(' || *ptr == ')') {
		i++;

		if (*ptr == '(') {
			floor++;
			ptr++;
			continue;
		}
		floor--;
		ptr++;

		if (floor == -1) {
			break;
		}
	}

	printf("%d\n", i);
	return 0;
}
