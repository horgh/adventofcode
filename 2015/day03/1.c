#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <map.h>
#include <stdio.h>
#include <string.h>

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char buf[8192] = {0};
	assert(fgets(buf, 8192, stdin) != NULL);

	struct htable * const h = hash_init(1024);
	assert(h != NULL);

	int x = 0;
	int y = 0;
	assert(hash_set(h, "0,0", NULL));

	char const * ptr = buf;
	while (*ptr != '\0' && *ptr != '\n') {
		switch (*ptr) {
		case '^':
			y++;
			break;
		case 'v':
			y--;
			break;
		case '>':
			x++;
			break;
		case '<':
			x--;
			break;
		default:
			assert(1 == 0);
		}
		ptr++;

		char s[128] = {0};
		sprintf(s, "%d,%d", x, y);
		assert(hash_set(h, s, NULL));
	}

	int const houses = hash_count_elements(h);
	printf("%d\n", houses);
	assert(hash_free(h, NULL));
	return 0;
}
