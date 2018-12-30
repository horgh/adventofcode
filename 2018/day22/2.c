#define _POSIX_C_SOURCE 200809L

#include <a_star.h>
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Type { Rocky, Narrow, Wet };

enum Tool { None, Torch, Climbing };

struct Position {
	enum Type type;
	uint64_t erosion_level;
	struct AStarNode * nodes[3];
};

static void draw(struct Position ** const, int const, int const, int const);

static void get_type(struct Position ** const,
		int const,
		int const,
		int const,
		int const,
		int const);

static uint64_t get_geologic_index(
		struct Position ** const, int const, int const, int const, int const);

static void print_map(struct Position ** const, int const, int const);

static uint64_t search(
		struct Position ** const, int64_t const, int64_t const, enum Tool const);

static uint64_t cost(
		struct AStarNode const * const, struct AStarNode const * const);

static uint64_t heuristic(
		struct AStarNode const * const, struct AStarNode const * const);

static struct AStarNode ** get_neighbours(struct AStarGraph const * const,
		struct AStarNode const * const,
		size_t * const);

static void get_neighbours_at(struct Position ** const,
		struct AStarNode const * const,
		struct AStarNode ** const,
		size_t * const,
		int64_t const,
		int64_t const);

#define MAP_SZ_X 250
#define MAP_SZ_Y 800
#define NEIGHBOURS_SZ 24

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

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

	struct Position ** const map = calloc(MAP_SZ_X, sizeof(struct Position *));
	assert(map != NULL);
	for (size_t i = 0; i < MAP_SZ_X; i++) {
		map[i] = calloc(MAP_SZ_Y, sizeof(struct Position));
		assert(map[i] != NULL);
	}

	draw(map, depth, target_x, target_y);

	if (0) {
		print_map(map, target_x, target_y);
	}

	uint64_t const steps =
			search(map, (int64_t)target_x, (int64_t)target_y, Torch);
	printf("%" PRIu64 "\n", steps);

	for (size_t i = 0; i < MAP_SZ_X; i++) {
		free(map[i]);
	}
	free(map);
	return 0;
}

static void
draw(struct Position ** const map,
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
get_type(struct Position ** const map,
		int const depth,
		int const target_x,
		int const target_y,
		int const x,
		int const y)
{
	uint64_t const geologic_index =
			get_geologic_index(map, target_x, target_y, x, y);
	uint64_t const erosion_level = (geologic_index + (uint64_t)depth) % 20183;
	map[x][y].erosion_level = erosion_level;
	uint64_t const m = erosion_level % 3;
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

__attribute__((pure)) static uint64_t
get_geologic_index(struct Position ** const map,
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
		return (uint64_t)x * 16807;
	}
	if (x == 0) {
		return (uint64_t)y * 48271;
	}
	assert(x - 1 >= 0);
	assert(y - 1 >= 0);
	return map[x - 1][y].erosion_level * map[x][y - 1].erosion_level;
}

static void
print_map(struct Position ** const map, int const target_x, int const target_y)
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

static uint64_t
search(struct Position ** const map,
		int64_t const target_x,
		int64_t const target_y,
		enum Tool const target_tool)
{
	struct AStarGraph * const graph =
			a_star_graph_create(MAP_SZ_X * MAP_SZ_Y * 3);
	graph->data = map;

	struct AStarNode * start = NULL;
	struct AStarNode * target = NULL;
	for (int64_t x = 0; x < MAP_SZ_X; x++) {
		for (int64_t y = 0; y < MAP_SZ_Y; y++) {
			for (int64_t t = 0; t < 3; t++) {
				struct AStarNode * const n = a_star_add_node_3(graph, x, y, t);

				map[x][y].nodes[t] = n;

				if (x == 0 && y == 0 && t == Torch) {
					start = n;
					continue;
				}
				if (x == target_x && y == target_y && t == target_tool) {
					target = n;
					continue;
				}
			}
		}
	}

	uint64_t const distance =
			a_star_search(graph, start, target, cost, heuristic, get_neighbours);
	a_star_graph_free(graph);
	return distance;
}

static uint64_t
cost(struct AStarNode const * const from, struct AStarNode const * const to)
{
	if (from->coords[2] != to->coords[2]) {
		return from->g + 7;
	}
	return from->g + 1;
}

static uint64_t
heuristic(
		struct AStarNode const * const from, struct AStarNode const * const to)
{
	uint64_t cost = (uint64_t)llabs(from->coords[0] - to->coords[0]) +
									(uint64_t)llabs(from->coords[1] - to->coords[1]);
	return cost;
}

static struct AStarNode **
get_neighbours(struct AStarGraph const * const graph,
		struct AStarNode const * const current,
		size_t * const n_neighbours)
{
	struct AStarNode ** neighbours =
			calloc(NEIGHBOURS_SZ, sizeof(struct AStarNode *));
	assert(neighbours != NULL);
	*n_neighbours = 0;
	int64_t const x = current->coords[0];
	int64_t const y = current->coords[1];
	struct Position ** const map = graph->data;
	get_neighbours_at(map, current, neighbours, n_neighbours, x, y - 1);
	get_neighbours_at(map, current, neighbours, n_neighbours, x + 1, y);
	get_neighbours_at(map, current, neighbours, n_neighbours, x, y + 1);
	get_neighbours_at(map, current, neighbours, n_neighbours, x - 1, y);
	return neighbours;
}

static void
get_neighbours_at(struct Position ** const map,
		struct AStarNode const * const current,
		struct AStarNode ** const neighbours,
		size_t * const n_neighbours,
		int64_t const new_x,
		int64_t const new_y)
{
	if (new_x < 0 || new_y < 0) {
		return;
	}
	if (new_x >= MAP_SZ_X || new_y >= MAP_SZ_Y) {
		printf("hit edge\n");
		exit(1);
	}

	int64_t const x = current->coords[0];
	int64_t const y = current->coords[1];
	enum Tool const tool = current->coords[2];

	enum Type const type = map[x][y].type;
	enum Type const new_type = map[new_x][new_y].type;

	size_t new_n_neighbours = *n_neighbours;

	if (new_type == Rocky) {
		if (type == Rocky || type == Wet) {
			if (tool == Climbing) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[Climbing];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[Climbing];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
		if (type == Rocky || type == Narrow) {
			if (tool == Torch) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[Torch];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[Torch];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
	}

	if (new_type == Wet) {
		if (type == Rocky || type == Wet) {
			if (tool == Climbing) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[Climbing];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[Climbing];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
		if (type == Wet || type == Narrow) {
			if (tool == None) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[None];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[None];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
	}

	if (new_type == Narrow) {
		if (type == Rocky || type == Narrow) {
			if (tool == Torch) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[Torch];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[Torch];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
		if (type == Wet || type == Narrow) {
			if (tool == None) {
				neighbours[new_n_neighbours++] = map[new_x][new_y].nodes[None];
			} else {
				neighbours[new_n_neighbours++] = map[x][y].nodes[None];
			}
			assert(new_n_neighbours != NEIGHBOURS_SZ);
		}
	}

	*n_neighbours = new_n_neighbours;
}
