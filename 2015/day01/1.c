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
	while (*ptr == '(' || *ptr == ')') {
		if (*ptr == '(') {
			floor++;
			ptr++;
			continue;
		}
		floor--;
		ptr++;
	}

	printf("%d\n", floor);
	return 0;
}
