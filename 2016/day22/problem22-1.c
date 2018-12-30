#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1024

struct Node {
	int x;
	int y;
	int size;
	int used;
	bool goal;
};

struct Step {
	struct Node * nodes;
	int steps;
};

static void __print_nodes(struct Node * const, const size_t);
static void __print_node(struct Node * const);
static int __count_pairs(struct Node * const, const size_t);
static struct Node * __find_top_right_node(struct Node * const, const size_t);
static int __solve(struct Node * const, const size_t);
static bool __enqueue(
		struct Queue * const, struct Node * const, const size_t, const int);
static void __destroy_step(void * const);
static bool __goal_is_here(struct Node * const, const size_t);
static bool __queue_moves(
		struct Queue * const, struct Node * const, const size_t, const int);
static bool __is_neighbour(
		const struct Node * const, const struct Node * const);
static char * __nodes_to_string(struct Node * const, const size_t);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	struct Node * const nodes = calloc(MAX_NODES, sizeof(struct Node));
	if (!nodes) {
		printf("%s\n", strerror(errno));
		return 1;
	}

	size_t i = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		const char * ptr = buf;

		if (*ptr != '/') {
			continue;
		}

		while (*ptr != 0 && strncmp(ptr, "node-x", strlen("node-x")) != 0) {
			ptr++;
		}

		if (*ptr == 0) {
			printf("unexpected input\n");
			fclose(fh);
			free(nodes);
			return 1;
		}

		ptr += strlen("node-x");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("unexpected input, x not found\n");
			fclose(fh);
			free(nodes);
			return 1;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		if (*ptr != '-' || *(ptr + 1) != 'y') {
			printf("unexpected input, after x: %s (line %zu)\n", ptr, i);
			fclose(fh);
			free(nodes);
			return 1;
		}

		ptr += 2;

		int y = -1;
		matches = sscanf(ptr, "%d", &y);
		if (matches != 1) {
			printf("unexpected input, y not found\n");
			fclose(fh);
			free(nodes);
			return 1;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		while (isspace(*ptr)) {
			ptr++;
		}

		int size = -1;
		matches = sscanf(ptr, "%d", &size);
		if (matches != 1) {
			printf("unexpected input, size not found\n");
			fclose(fh);
			free(nodes);
			return 1;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		if (*ptr != 'T') {
			printf("unexpected lack of T");
			fclose(fh);
			free(nodes);
			return 1;
		}

		ptr++;

		int used = -1;
		matches = sscanf(ptr, "%d", &used);
		if (matches != 1) {
			printf("unexpected input, used not found\n");
			fclose(fh);
			free(nodes);
			return 1;
		}

		struct Node * const node = nodes + i;
		i++;

		node->x = x;
		node->y = y;
		node->size = size;
		node->used = used;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		free(nodes);
		return 1;
	}

	if (0) {
		__print_nodes(nodes, i);
	}

	const int pairs = __count_pairs(nodes, i);
	printf("%d\n", pairs);

	struct Node * const top_right = __find_top_right_node(nodes, i);
	if (!top_right) {
		printf("top right node not found\n");
		free(nodes);
		return 1;
	}

	if (0) {
		printf("top right node: ");
		__print_node(top_right);
	}
	top_right->goal = true;

	const int steps = __solve(nodes, i);
	printf("%d\n", steps);

	free(nodes);
	return 0;
}

static void
__print_nodes(struct Node * const nodes, const size_t n)
{
	for (size_t i = 0; i < n; i++) {
		struct Node * const node = nodes + i;
		__print_node(node);
	}
}

static void
__print_node(struct Node * const node)
{
	printf("Node (x=%d, y=%d) Size: %dT Used: %dT Avail: %dT\n",
			node->x,
			node->y,
			node->size,
			node->used,
			node->size - node->used);
}

static int
__count_pairs(struct Node * const nodes, const size_t n)
{
	int pairs = 0;

	for (size_t i = 0; i < n; i++) {
		struct Node * const node0 = nodes + i;
		if (node0->used == 0) {
			continue;
		}

		for (size_t j = 0; j < n; j++) {
			if (i == j) {
				continue;
			}

			struct Node * const node1 = nodes + j;

			const int avail = node1->size - node1->used;

			if (avail >= node0->used) {
				pairs++;
			}
		}
	}

	return pairs;
}

static struct Node *
__find_top_right_node(struct Node * const nodes, const size_t n)
{
	struct Node * top_right = NULL;

	for (size_t i = 0; i < n; i++) {
		struct Node * const node = nodes + i;

		if (node->y != 0) {
			continue;
		}

		if (!top_right) {
			top_right = node;
			continue;
		}

		if (node->x > top_right->x) {
			top_right = node;
		}
	}

	return top_right;
}

static int
__solve(struct Node * const nodes, const size_t sz)
{
	struct Queue * const q = calloc(1, sizeof(struct Queue));
	if (!q) {
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (!__enqueue(q, nodes, sz, 0)) {
		printf("__enqueue\n");
		destroy_queue(q, __destroy_step);
		return -1;
	}

	struct htable * const visited = hash_init(1000);
	if (!visited) {
		printf("hash_init\n");
		destroy_queue(q, __destroy_step);
		return -1;
	}

	while (q->first) {
		struct Step * const step = dequeue(q);
		if (!step) {
			printf("dequeue\n");
			destroy_queue(q, __destroy_step);
			hash_free(visited, NULL);
			return -1;
		}

		if (__goal_is_here(step->nodes, sz)) {
			// printf("goal at 0,0\n");
			const int steps = step->steps;
			destroy_queue(q, __destroy_step);
			hash_free(visited, NULL);
			__destroy_step(step);
			return steps;
		}

		char * const nodes_str = __nodes_to_string(step->nodes, sz);
		if (!nodes_str) {
			printf("__nodes_to_string\n");
			destroy_queue(q, __destroy_step);
			hash_free(visited, NULL);
			__destroy_step(step);
			return -1;
		}

		// printf("%s\n", nodes_str);

		if (hash_has_key(visited, nodes_str)) {
			__destroy_step(step);
			free(nodes_str);
			continue;
		}

		if (!hash_set(visited, nodes_str, NULL)) {
			printf("hash_set\n");
			destroy_queue(q, __destroy_step);
			hash_free(visited, NULL);
			__destroy_step(step);
			free(nodes_str);
			return -1;
		}

		if (!__queue_moves(q, step->nodes, sz, step->steps)) {
			printf("__queue_moves\n");
			destroy_queue(q, __destroy_step);
			hash_free(visited, NULL);
			__destroy_step(step);
			free(nodes_str);
			return -1;
		}

		__destroy_step(step);
		free(nodes_str);
	}

	destroy_queue(q, __destroy_step);
	hash_free(visited, NULL);
	return -1;
}

static bool
__enqueue(struct Queue * const q,
		struct Node * const nodes,
		const size_t sz,
		const int steps)
{
	// Copy the nodes

	struct Node * const nodes2 = calloc(sz, sizeof(struct Node));
	if (!nodes2) {
		printf("%s\n", strerror(errno));
		return false;
	}

	for (size_t i = 0; i < sz; i++) {
		struct Node * const node1 = nodes + i;
		struct Node * const node2 = nodes2 + i;
		node2->x = node1->x;
		node2->y = node1->y;
		node2->size = node1->size;
		node2->used = node1->used;
		node2->goal = node1->goal;
	}

	struct Step * const step = calloc(1, sizeof(struct Step));
	if (!step) {
		printf("%s\n", strerror(errno));
		free(nodes2);
		return false;
	}

	step->nodes = nodes2;
	step->steps = steps;

	if (!enqueue(q, step)) {
		printf("enqueue\n");
		free(nodes2);
		free(step);
		return false;
	}

	return true;
}

static void
__destroy_step(void * const p)
{
	if (!p) {
		return;
	}

	struct Step * const step = p;

	if (step->nodes) {
		free(step->nodes);
	}

	free(step);
}

static bool
__goal_is_here(struct Node * const nodes, const size_t sz)
{
	for (size_t i = 0; i < sz; i++) {
		struct Node * const node = nodes + i;

		if (!node->goal) {
			continue;
		}

		return node->x == 0 && node->y == 0;
	}

	return false;
}

static bool
__queue_moves(struct Queue * const q,
		struct Node * const nodes,
		const size_t sz,
		const int steps)
{
	// Queue all viable pairs.

	for (size_t i = 0; i < sz; i++) {
		struct Node * const node0 = nodes + i;
		if (node0->used == 0) {
			continue;
		}

		for (size_t j = 0; j < sz; j++) {
			if (i == j) {
				continue;
			}

			struct Node * const node1 = nodes + j;

			const int avail = node1->size - node1->used;

			// Try only moving to the empty node. We will find a solution either way
			// though.
			if (node1->used != 0) {
				continue;
			}

			if (avail < node0->used) {
				continue;
			}

			// Can only transfer to neighbours.
			if (!__is_neighbour(node0, node1)) {
				continue;
			}

			// Temporarily change. Queue will make a copy.
			const int orig_used = node0->used;
			const bool orig_goal0 = node0->goal;
			const bool orig_goal1 = node1->goal;

			node1->used += node0->used;
			node0->used = 0;
			if (!node1->goal) {
				node1->goal = node0->goal;
			}
			node0->goal = false;

			if (!__enqueue(q, nodes, sz, steps + 1)) {
				printf("__enqueue\n");
				return false;
			}

			// Revert change.
			node1->used -= orig_used;
			node0->used = orig_used;
			node1->goal = orig_goal1;
			node0->goal = orig_goal0;
		}
	}

	return true;
}

static bool
__is_neighbour(const struct Node * const node0, const struct Node * const node1)
{
	// Node1 is to right?
	if (node0->x + 1 == node1->x && node0->y == node1->y) {
		return true;
	}

	// Node1 is to left?
	if (node0->x - 1 == node1->x && node0->y == node1->y) {
		return true;
	}

	// Node1 is above?
	if (node0->x == node1->x && node0->y - 1 == node1->y) {
		return true;
	}

	// Node1 is below?
	if (node0->x == node1->x && node0->y + 1 == node1->y) {
		return true;
	}

	return false;
}

static char *
__nodes_to_string(struct Node * const nodes, const size_t sz)
{
	char * const s = calloc(256, sizeof(char));
	if (!s) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	int goal_x = -1;
	int goal_y = -1;

	char buf[256];
	memset(buf, 0, 256);

	for (size_t i = 0; i < sz; i++) {
		const struct Node * const node = nodes + i;

		if (node->goal) {
			goal_x = node->x;
			goal_y = node->y;
			continue;
		}

		if (node->used != 0) {
			continue;
		}

		memset(buf, 0, 256);
		sprintf(buf, "E(%d,%d)", node->x, node->y);

		strcat(s, buf);
	}

	memset(buf, 0, 256);
	sprintf(buf, "G(%d,%d)", goal_x, goal_y);

	strcat(s, buf);

	return s;
}
