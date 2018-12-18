#define _POSIX_C_SOURCE 200810L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum AcreType { Open, Trees, Lumber };

struct Position {
	enum AcreType type;
};

static void
print_map(struct Position * * const, int const);

static void
tick(struct Position * * const, int const);

static int
count(struct Position * * const,
		int const,
		enum AcreType const,
		int const,
		int const);

#define SZ 50

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Position * * const map = calloc(SZ, sizeof(struct Position *));
	assert(map != NULL);
	for (size_t i = 0; i < SZ; i++) {
		map[i] = calloc(SZ, sizeof(struct Position));
		assert(map[i] != NULL);
	}
	int y = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		int x = 0;
		while (*ptr != '\0') {
			if (*ptr == '.') {
				map[x][y].type = Open;
			} else if (*ptr == '|') {
				map[x][y].type = Trees;
			} else if (*ptr == '#') {
				map[x][y].type = Lumber;
			} else {
				assert(1 == 0);
			}
			ptr++;
			x++;
		}
		y++;
	}

	if (0) {
		print_map(map, y);
	}

	int const minutes = 10;
	for (int i = 0; i < minutes; i++) {
		tick(map, y);
	}

	int trees = 0;
	int lumber = 0;
	for (int y2 = 0; y2 < y; y2++) {
		for (int x = 0; x < y; x++) {
			if (map[x][y2].type == Trees) {
				trees++;
				continue;
			}
			if (map[x][y2].type == Lumber) {
				lumber++;
				continue;
			}
		}
	}

	for (size_t i = 0; i < SZ; i++) {
		free(map[i]);
	}
	free(map);
	printf("%d\n", trees*lumber);
	return 0;
}

static void
print_map(struct Position * * const map, int const sz)
{
	for (int y = 0; y < sz; y++) {
		for (int x = 0; x < sz; x++) {
			if (map[x][y].type == Open) {
				printf(".");
				continue;
			}
			if (map[x][y].type == Trees) {
				printf("|");
				continue;
			}
			if (map[x][y].type == Lumber) {
				printf("#");
				continue;
			}
			assert(1 == 0);
		}
		printf("\n");
	}
}

static void
tick(struct Position * * const map, int const sz)
{
	struct Position * * const map2 = calloc(SZ, sizeof(struct Position *));
	assert(map2 != NULL);
	for (size_t i = 0; i < SZ; i++) {
		map2[i] = calloc(SZ, sizeof(struct Position));
		assert(map2[i] != NULL);
		memcpy(map2[i], map[i], SZ*sizeof(struct Position));
	}

	for (int y = 0; y < sz; y++) {
		for (int x = 0; x < sz; x++) {
			if (map2[x][y].type == Open) {
				int const trees = count(map2, sz, Trees, x, y);
				if (trees >= 3) {
					map[x][y].type = Trees;
				}
				continue;
			}
			if (map2[x][y].type == Trees) {
				int const lumber = count(map2, sz, Lumber, x, y);
				if (lumber >= 3) {
					map[x][y].type = Lumber;
				}
				continue;
			}
			if (map2[x][y].type == Lumber) {
				int const trees = count(map2, sz, Trees, x, y);
				int const lumber = count(map2, sz, Lumber, x, y);
				if (trees > 0 && lumber > 0) {
				} else {
					map[x][y].type = Open;
				}
				continue;
			}
			assert(1 == 0);
		}
	}

	for (size_t i = 0; i < SZ; i++) {
		free(map2[i]);
	}
	free(map2);
}

static int
count(struct Position * * const map,
		int const sz,
		enum AcreType const type,
		int const x,
		int const y)
{
	int c = 0;
	if (x-1 >= 0 && y-1 >= 0) {
		if (map[x-1][y-1].type == type) {
			c++;
		}
	}
	if (y-1 >= 0) {
		if (map[x][y-1].type == type) {
			c++;
		}
	}
	if (x+1 != sz && y-1 >= 0) {
		if (map[x+1][y-1].type == type) {
			c++;
		}
	}
	if (x-1 >= 0) {
		if (map[x-1][y].type == type) {
			c++;
		}
	}
	if (x+1 != sz) {
		if (map[x+1][y].type == type) {
			c++;
		}
	}
	if (x-1 >= 0 && y+1 != sz) {
		if (map[x-1][y+1].type == type) {
			c++;
		}
	}
	if (y+1 != sz) {
		if (map[x][y+1].type == type) {
			c++;
		}
	}
	if (x+1 != sz && y+1 != sz) {
		if (map[x+1][y+1].type == type) {
			c++;
		}
	}
	return c;
}
