#ifndef ADVENT_A_STAR_H
#define ADVENT_A_STAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct AStarNode {
	int64_t coords[3]; // 3 is arbitrary, 3 dimensions.
	uint64_t g;				 // g(n).
	uint64_t h;				 // h(n).
	bool visited;
};

struct AStarGraph {
	struct AStarNode ** nodes;
	size_t n_nodes;
	void * data; // Problem specific data to pass around.
};

struct AStarGraph * a_star_graph_create(size_t const n_nodes);

struct AStarNode * a_star_add_node_3(struct AStarGraph * const graph,
		int64_t const a,
		int64_t const b,
		int64_t const c);

void a_star_graph_free(struct AStarGraph * const graph);

// Find shortest path from start to target. Return the path's cost.
//
// If the heuristic function returns 0 then this is Dijkstra's algorithm.
//
// If the heuristic function returns something non-zero then it is A*.
uint64_t a_star_search(struct AStarGraph * const graph,
		struct AStarNode * const start,
		struct AStarNode const * const target,
		uint64_t (*const cost)(
				struct AStarNode const * const, struct AStarNode const * const),
		uint64_t (*const heuristic)(
				struct AStarNode const * const, struct AStarNode const * const),
		struct AStarNode ** (*const get_neighbours)(struct AStarGraph const * const,
				struct AStarNode const * const,
				size_t * const));

#endif
