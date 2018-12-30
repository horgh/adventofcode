#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <map.h>
#include <stdbool.h>
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

	int santa_x = 0;
	int santa_y = 0;
	int robo_santa_x = 0;
	int robo_santa_y = 0;
	assert(hash_set(h, "0,0", NULL));

	char const * ptr = buf;
	bool santa = true;
	while (*ptr != '\0' && *ptr != '\n') {
		switch (*ptr) {
		case '^':
			if (santa) {
				santa_y++;
			} else {
				robo_santa_y++;
			}
			break;
		case 'v':
			if (santa) {
				santa_y--;
			} else {
				robo_santa_y--;
			}
			break;
		case '>':
			if (santa) {
				santa_x++;
			} else {
				robo_santa_x++;
			}
			break;
		case '<':
			if (santa) {
				santa_x--;
			} else {
				robo_santa_x--;
			}
			break;
		default:
			assert(1 == 0);
		}
		ptr++;

		char s[128] = {0};
		if (santa) {
			sprintf(s, "%d,%d", santa_x, santa_y);
		} else {
			sprintf(s, "%d,%d", robo_santa_x, robo_santa_y);
		}
		santa = !santa;
		assert(hash_set(h, s, NULL));
	}

	int const houses = hash_count_elements(h);
	printf("%d\n", houses);
	assert(hash_free(h, NULL));
	return 0;
}
