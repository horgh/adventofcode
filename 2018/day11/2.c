#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#define SZ 300

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <serial>\n", argv[0]);
		return 1;
	}
	int const serial = atoi(argv[1]);

	int grid[SZ][SZ] = {0};

	for (int y = 0; y < SZ; y++) {
		for (int x = 0; x < SZ; x++) {
			int const rack_id = x+1+10;
			int power_level = rack_id*(y+1);
			power_level += serial;
			power_level *= rack_id;
			if (power_level < 100) {
				power_level = 0;
			} else {
				power_level = power_level % 1000 / 100;
			}
			power_level -= 5;
			grid[x][y] = power_level;
		}
	}

	int max_power = 0;
	int max_x = 0;
	int max_y = 0;
	int max_sz = 0;
	int grid_acc[SZ][SZ] = {0};
	for (int y = 0; y < SZ; y++) {
		for (int x = 0; x < SZ; x++) {
			grid_acc[x][y] = grid[x][y];
		}
	}

	for (int sz = 2; sz <= SZ; sz++) {
		for (int y = 0; y < SZ; y++) {
			if (y+sz >= SZ) {
				continue;
			}
			for (int x = 0; x < SZ; x++) {
				if (x+sz >= SZ) {
					continue;
				}
				int power = grid_acc[x][y];
				for (int yy = y; yy < y+sz-1; yy++) {
					power += grid[x+sz-1][yy];
				}
				for (int xx = x; xx < x+sz; xx++) {
					power += grid[xx][y+sz-1];
				}
				grid_acc[x][y] = power;
				if (power <= max_power) {
					continue;
				}
				max_power = power;
				max_x = x;
				max_y = y;
				max_sz = sz;
			}
		}
	}

	printf("%d,%d,%d\n", max_x+1, max_y+1, max_sz);
	return 0;
}
