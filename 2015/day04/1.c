#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <hash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char buf[8192] = {0};
	assert(fgets(buf, 8192, stdin) != NULL);
	trim_right(buf);

	int i = 0;
	while (1) {
		char s[128] = {0};
		sprintf(s, "%s%d", buf, i);

		unsigned char * const h = md5(s);
		assert(h != NULL);

		if (h[0] == 0 && h[1] == 0 && (h[2] & 0xf0) == 0) {
			free(h);
			break;
		}

		free(h);
		i++;
	}

	printf("%d\n", i);
	return 0;
}
