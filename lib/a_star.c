#include "a_star.h"
#include <assert.h>
#include <stdlib.h>

#define A_STAR_INFINITY UINT64_MAX

static struct AStarNode *
choose_next_node(struct AStarGraph * const graph);

static void
update_costs(struct AStarGraph const * const graph,
		struct AStarNode const * const current,
		struct AStarNode const * const target,
		uint64_t (* const cost)(struct AStarNode const * const,
			struct AStarNode const * const),
		uint64_t (* const heuristic)(struct AStarNode const * const,
			struct AStarNode const * const),
		struct AStarNode * * (* const get_neighbours)(
			struct AStarGraph const * const,
			struct AStarNode const * const,
			size_t * const));

struct AStarGraph *
a_star_graph_create(size_t const n_nodes)
{
	struct AStarGraph * const graph = calloc(1, sizeof(struct AStarGraph));
	assert(graph != NULL);
	graph->nodes = calloc(n_nodes, sizeof(struct AStarNode *));
	assert(graph->nodes != NULL);
	return graph;
}

struct AStarNode *
a_star_add_node_3(struct AStarGraph * const graph,
		int64_t const a,
		int64_t const b,
		int64_t const c)
{
	struct AStarNode * const n = calloc(1, sizeof(struct AStarNode));
	assert(n != NULL);
	n->coords[0] = a;
	n->coords[1] = b;
	n->coords[2] = c;
	graph->nodes[graph->n_nodes++] = n;
	return n;
}

void
a_star_graph_free(struct AStarGraph * const graph)
{
	for (size_t i = 0; i < graph->n_nodes; i++) {
		free(graph->nodes[i]);
	}
	free(graph->nodes);
	free(graph);
}

uint64_t
a_star_search(struct AStarGraph * const graph,
		struct AStarNode * const start,
		struct AStarNode const * const target,
		uint64_t (* const cost)(struct AStarNode const * const,
			struct AStarNode const * const),
		uint64_t (* const heuristic)(struct AStarNode const * const,
			struct AStarNode const * const),
		struct AStarNode * * (* const get_neighbours)(
			struct AStarGraph const * const,
			struct AStarNode const * const,
			size_t * const))
{
	for (size_t i = 0; i < graph->n_nodes; i++) {
		graph->nodes[i]->visited = false;
		graph->nodes[i]->g = A_STAR_INFINITY;
	}

	start->g = 0;
	start->h = heuristic(start, target);

	while (1) {
		struct AStarNode * const current = choose_next_node(graph);
		if (!current) {
			return 0; // No path.
		}

		if (current == target) {
			return current->g;
		}

		update_costs(graph, current, target, cost, heuristic, get_neighbours);
		current->visited = true;
	}
}

static struct AStarNode *
choose_next_node(struct AStarGraph * const graph)
{
	struct AStarNode * cheapest = NULL;
	uint64_t best_f = A_STAR_INFINITY;

	for (size_t i = 0; i < graph->n_nodes; i++) {
		struct AStarNode * const n = graph->nodes[i];
		if (n->visited) {
			continue;
		}

		if (n->g == A_STAR_INFINITY) {
			continue;
		}

		uint64_t const f = n->g + n->h;
		if (f >= best_f) {
			continue;
		}

		best_f = f;
		cheapest = n;
	}

	return cheapest;
}

static void
update_costs(struct AStarGraph const * const graph,
		struct AStarNode const * const current,
		struct AStarNode const * const target,
		uint64_t (* const cost)(struct AStarNode const * const,
			struct AStarNode const * const),
		uint64_t (* const heuristic)(struct AStarNode const * const,
			struct AStarNode const * const),
		struct AStarNode * * (* const get_neighbours)(
			struct AStarGraph const * const,
			struct AStarNode const * const,
			size_t * const))
{
	size_t n_neighbours = 0;
	struct AStarNode * * const neighbours = get_neighbours(graph, current,
			&n_neighbours);
	for (size_t i = 0; i < n_neighbours; i++) {
		struct AStarNode * const neighbour = neighbours[i];
		uint64_t const g = cost(current, neighbour);
		if (g >= neighbour->g) {
			continue;
		}
		neighbour->g = g;
		neighbour->h = heuristic(neighbour, target);
	}
	free(neighbours);
}
