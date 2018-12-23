#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Type { Rocky, Narrow, Wet };

enum Tool { None, Torch, Climbing };

struct Position {
	enum Type type;
	uint64_t erosion_level;

	bool visited[3];
	int distance[3];
	int distance_heuristic[3];
};

struct Candidate {
	int x;
	int y;
	enum Tool tool;
	int distance;
};

static void
draw(struct Position * * const,
		int const,
		int const,
		int const);

static void
get_type(struct Position * * const,
		int const,
		int const,
		int const,
		int const,
		int const);

static uint64_t
get_geologic_index(struct Position * * const,
		int const,
		int const,
		int const,
		int const);

static void
print_map(struct Position * * const,
		int const,
		int const);

static int
dijkstra(struct Position * * const,
		int const,
		int const);

static int
heuristic(int const,
		int const,
		int const,
		int const);

static void
update_distances(struct Position * * const,
		int const,
		int const,
		enum Tool const,
		int const,
		int const);

static void
update_distance_to(struct Position * * const,
		int const,
		int const,
		enum Tool const,
		int const,
		int const,
		int const,
		int const);

static void
set_distance(struct Position * * const,
		int const,
		int const,
		enum Tool const,
		int const,
		int const,
		int const,
		int const,
		enum Tool const);

static struct Candidate
choose_next(struct Position * * const);

#define MAP_SZ_X 250
#define MAP_SZ_Y 800

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	char buf[4096] = {0};

	assert(fgets(buf, 4096, stdin) != NULL);
	trim_right(buf);
	char const * ptr = buf;
	ptr += strlen("depth: ");
	int const depth = atoi(ptr);
	memset(buf, 0, 4096);

	assert(fgets(buf, 4096, stdin) != NULL);
	trim_right(buf);
	ptr = buf;
	ptr += strlen("target: ");
	int const target_x = atoi(ptr);
	while (isdigit(*ptr)) {
		ptr++;
	}
	ptr++;
	int const target_y = atoi(ptr);

	if (0) {
		printf("depth: %d target: %d,%d\n", depth, target_x, target_y);
	}

	struct Position * * const map = calloc(MAP_SZ_X,
			sizeof(struct Position *));
	assert(map != NULL);
	for (size_t i = 0; i < MAP_SZ_X; i++) {
		map[i] = calloc(MAP_SZ_Y, sizeof(struct Position));
		assert(map[i] != NULL);
	}

	draw(map, depth, target_x, target_y);

	if (0) {
		print_map(map, target_x, target_y);
	}

	int const steps = dijkstra(map, target_x, target_y);
	printf("%d\n", steps);
	return 0;
}

static void
draw(struct Position * * const map,
		int const depth,
		int const target_x,
		int const target_y)
{
	for (int y = 0; y < MAP_SZ_Y; y++) {
		for (int x = 0; x < MAP_SZ_X; x++) {
			get_type(map, depth, target_x, target_y, x, y);
		}
	}
}

static void
get_type(struct Position * * const map,
		int const depth,
		int const target_x,
		int const target_y,
		int const x,
		int const y)
{
	uint64_t const geologic_index = get_geologic_index(map, target_x,
			target_y, x, y);
	uint64_t const erosion_level = (geologic_index+(uint64_t) depth)%20183;
	map[x][y].erosion_level = erosion_level;
	uint64_t const m = erosion_level%3;
	if (m == 0) {
		map[x][y].type = Rocky;
		return;
	}
	if (m == 1) {
		map[x][y].type = Wet;
		return;
	}
	map[x][y].type = Narrow;
}

__attribute__((pure))
static uint64_t
get_geologic_index(struct Position * * const map,
		int const target_x,
		int const target_y,
		int const x,
		int const y)
{
	if (x == 0 && y == 0) {
		return 0;
	}
	if (x == target_x && y == target_y) {
		return 0;
	}
	if (y == 0) {
		return (uint64_t) x*16807;
	}
	if (x == 0) {
		return (uint64_t) y*48271;
	}
	assert(x-1 >= 0);
	assert(y-1 >= 0);
	return map[x-1][y].erosion_level*map[x][y-1].erosion_level;
}

static void
print_map(struct Position * * const map,
		int const target_x,
		int const target_y)
{
	for (int y = 0; y < MAP_SZ_Y; y++) {
		for (int x = 0; x < MAP_SZ_X; x++) {
			if (x == 0 && y == 0) {
				printf("M");
				continue;
			}
			if (x == target_x && y == target_y) {
				printf("T");
				continue;
			}
			if (map[x][y].type == Rocky) {
				printf(".");
				continue;
			}
			if (map[x][y].type == Wet) {
				printf("=");
				continue;
			}
			if (map[x][y].type == Narrow) {
				printf("|");
				continue;
			}
			assert(1 == 0);
		}
		printf("\n");
	}
}

static int
dijkstra(struct Position * * const map,
		int const target_x,
		int const target_y)
{
#define MYINFINITY 99999
	for (int y = 0; y < MAP_SZ_Y; y++) {
		for (int x = 0; x < MAP_SZ_X; x++) {
			for (size_t i = 0; i < 3; i++) {
				map[x][y].distance[i] = MYINFINITY;
			}
		}
	}

	map[0][0].distance[Torch] = 0;
	map[0][0].distance_heuristic[Torch] = heuristic(0, 0, target_x, target_y);

	while (1) {
		struct Candidate const c = choose_next(map);
		if (c.distance == MYINFINITY) {
			return -1;
		}
		if (c.x == target_x && c.y == target_y && c.tool == Torch) {
			return c.distance;
		}

		update_distances(map, c.x, c.y, c.tool, target_x, target_y);
		map[c.x][c.y].visited[c.tool] = true;
	}
}

static int
heuristic(int const x0,
		int const y0,
		int const x1,
		int const y1)
{
	int const diff_x = x0-x1;
	int const diff_y = y0-y1;
	double const r = sqrt(diff_x*diff_x+diff_y*diff_y);
	return (int) r;
}

static void
update_distances(struct Position * * const map,
		int const x,
		int const y,
		enum Tool const tool,
		int const target_x,
		int const target_y)
{
	update_distance_to(map, x, y, tool, target_x, target_y, x, y-1);
	update_distance_to(map, x, y, tool, target_x, target_y, x+1, y);
	update_distance_to(map, x, y, tool, target_x, target_y, x, y+1);
	update_distance_to(map, x, y, tool, target_x, target_y, x-1, y);
}

static void
update_distance_to(struct Position * * const map,
		int const x,
		int const y,
		enum Tool const tool,
		int const target_x,
		int const target_y,
		int const new_x,
		int const new_y)
{
	if (new_x < 0 || new_y < 0) {
		return;
	}
	if (new_x >= MAP_SZ_X || new_y >= MAP_SZ_Y) {
		printf("hit edge\n");
		exit(1);
	}

	enum Type const type = map[x][y].type;
	enum Type const new_type = map[new_x][new_y].type;

	if (new_type == Rocky) {
		if (type == Rocky || type == Wet) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, Climbing);
		}
		if (type == Rocky || type == Narrow) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, Torch);
		}
	}

	if (new_type == Wet) {
		if (type == Rocky || type == Wet) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, Climbing);
		}
		if (type == Wet || type == Narrow) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, None);
		}
	}

	if (new_type == Narrow) {
		if (type == Rocky || type == Narrow) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, Torch);
		}
		if (type == Wet || type == Narrow) {
			set_distance(map, x, y, tool, target_x, target_y, new_x, new_y, None);
		}
	}
}

static void
set_distance(struct Position * * const map,
		int const x,
		int const y,
		enum Tool const tool,
		int const target_x,
		int const target_y,
		int const new_x,
		int const new_y,
		enum Tool const new_tool)
{
	if (tool != new_tool) {
		int const cost = map[x][y].distance[tool]+7;
		if (cost < map[x][y].distance[new_tool]) {
			map[x][y].distance[new_tool] = cost;
			map[x][y].distance_heuristic[new_tool] = cost+heuristic(x, y,
					target_x, target_y);
		}
		return;
	}

	int const cost = map[x][y].distance[tool]+1;
	if (cost < map[new_x][new_y].distance[new_tool]) {
		map[new_x][new_y].distance[new_tool] = cost;
		map[new_x][new_y].distance_heuristic[new_tool] = cost+heuristic(new_x, new_y,
				target_x, target_y);
	}
}

static struct Candidate
choose_next(struct Position * * const map)
{
	struct Candidate c = {
		.distance = MYINFINITY,
	};

	int best_heuristic = MYINFINITY;

	for (int y = 0; y < MAP_SZ_Y; y++) {
		for (int x = 0; x < MAP_SZ_X; x++) {
			for (size_t i = 0; i < 3; i++) {
				if (map[x][y].visited[i]) {
					continue;
				}
				if (map[x][y].distance[i] == MYINFINITY) {
					continue;
				}
				if (map[x][y].distance_heuristic[i] >= best_heuristic) {
					continue;
				}
				best_heuristic = map[x][y].distance_heuristic[i];
				c.x = x;
				c.y = y;
				c.tool = i;
				c.distance = map[x][y].distance[i];
			}
		}
	}

	return c;
}
