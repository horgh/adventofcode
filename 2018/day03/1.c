#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Claim {
	int id;
	int left;
	int top;
	int width;
	int height;
};

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Claim claims[2048] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;

		ptr++;

		claims[n].id = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}

		ptr += 3;

		claims[n].left = atoi(ptr);
		while(isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		claims[n].top = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += 2;
		claims[n].width = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		claims[n].height = atoi(ptr);

		n++;
	}

	int fabric[1024][1024] = {0};
	for (size_t i = 0; i < n; i++) {
		struct Claim c = claims[i];
		for (int x = c.left; x < c.left + c.width; x++) {
			for (int y = c.top; y < c.top + c.height; y++) {
				fabric[x][y]++;
			}
		}
	}

	int count = 0;
	for (int x = 0; x < 1024; x++) {
		for (int y = 0; y< 1024; y++) {
			if (fabric[x][y] >= 2) {
				count++;
			}
		}
	}

	printf("%d\n", count);
	return 0;
}
