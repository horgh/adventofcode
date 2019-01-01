#include "a_star.h"
#include "heap.h"
#include <assert.h>
#include <stdlib.h>

#define A_STAR_INFINITY INT64_MAX

static struct AStarNode * choose_next_node(struct Heap * const heap);

static void update_costs(struct AStarGraph const * const graph,
		struct Heap * const heap,
		struct AStarNode const * const current,
		struct AStarNode const * const target,
		int64_t (*const cost)(
				struct AStarNode const * const, struct AStarNode const * const),
		int64_t (*const heuristic)(
				struct AStarNode const * const, struct AStarNode const * const),
		struct AStarNode ** (*const get_neighbours)(struct AStarGraph const * const,
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

int64_t
a_star_search(struct AStarGraph * const graph,
		struct AStarNode * const start,
		struct AStarNode const * const target,
		int64_t (*const cost)(
				struct AStarNode const * const, struct AStarNode const * const),
		int64_t (*const heuristic)(
				struct AStarNode const * const, struct AStarNode const * const),
		struct AStarNode ** (*const get_neighbours)(struct AStarGraph const * const,
				struct AStarNode const * const,
				size_t * const))
{
	struct Heap * const heap = heap_create(graph->n_nodes);
	assert(heap != NULL);

	for (size_t i = 0; i < graph->n_nodes; i++) {
		struct AStarNode * const n = graph->nodes[i];

		n->visited = false;
		if (n == start) {
			n->g = 0;
			n->h = heuristic(start, target);
		} else {
			n->g = A_STAR_INFINITY;
		}

		struct HeapElement * const he = heap_insert(heap, n->g, n);
		assert(he != NULL);
		n->he = he;
	}

	while (1) {
		struct AStarNode * const current = choose_next_node(heap);
		if (!current) {
			heap_free(heap);
			return 0; // No path.
		}

		if (current == target) {
			heap_free(heap);
			return current->g;
		}

		update_costs(graph, heap, current, target, cost, heuristic, get_neighbours);
		current->visited = true;
	}
}

static struct AStarNode *
choose_next_node(struct Heap * const heap)
{
	struct AStarNode * const cheapest = heap_extract(heap);
	return cheapest;
}

static void
update_costs(struct AStarGraph const * const graph,
		struct Heap * const heap,
		struct AStarNode const * const current,
		struct AStarNode const * const target,
		int64_t (*const cost)(
				struct AStarNode const * const, struct AStarNode const * const),
		int64_t (*const heuristic)(
				struct AStarNode const * const, struct AStarNode const * const),
		struct AStarNode ** (*const get_neighbours)(struct AStarGraph const * const,
				struct AStarNode const * const,
				size_t * const))
{
	size_t n_neighbours = 0;
	struct AStarNode ** const neighbours =
			get_neighbours(graph, current, &n_neighbours);
	for (size_t i = 0; i < n_neighbours; i++) {
		struct AStarNode * const neighbour = neighbours[i];
		int64_t const g = cost(current, neighbour);
		if (g >= neighbour->g) {
			continue;
		}
		neighbour->g = g;
		neighbour->h = heuristic(neighbour, target);
		heap_decrease_priority(heap, neighbour->he, neighbour->g + neighbour->h);
	}
	free(neighbours);
}
