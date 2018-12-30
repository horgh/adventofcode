#include <assert.h>
#include <errno.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 1000000000

#define FLOOR_COUNT 4

// Define one of these to decide which input to use.
// Part 1 takes about 3 seconds.
// Part 2 takes several minutes and up to ~7.3GiB memory.
//#define INPUT_SIMPLE
//#define INPUT_SAMPLE
#define INPUT_PART1
//#define INPUT_PART2

// Now define what each input is.

#ifdef INPUT_SIMPLE
#define ELEMENT_COUNT 1

#define HYDROGEN 0
#endif

#ifdef INPUT_SAMPLE
#define ELEMENT_COUNT 2

#define HYDROGEN 0
#define LITHIUM 1
#endif

#ifdef INPUT_PART1
#define ELEMENT_COUNT 5

#define PROMETHIUM 0
#define COBALT 1
#define CURIUM 2
#define RUTHENIUM 3
#define PLUTONIUM 4
#endif

#ifdef INPUT_PART2
#define ELEMENT_COUNT 7

#define PROMETHIUM 0
#define COBALT 1
#define CURIUM 2
#define RUTHENIUM 3
#define PLUTONIUM 4
#define ELERIUM 5
#define DILITHIUM 6
#endif

enum Direction { UP, DOWN };

struct Floor {
	int number;
	bool elevator;
	bool * generators;
	bool * microchips;
	size_t num_generators;
	size_t num_microchips;
};

struct FloorsAndCost {
	struct Floor ** floors;
	int cost;
};

static void __destroy_floors(struct Floor ** const);
static void __destroy_floor(struct Floor * const);
static void __print_floors(struct Floor * const * const);
static void __print_floor(const struct Floor * const);
static int __solve_bfs(struct Floor ** const);
static void __get_moves_bfs(struct Queue * const,
		struct Floor ** const,
		struct Floor * const,
		struct Floor * const,
		const int);
static void __move_generator(
		struct Floor * const, struct Floor * const, const int);
static void __move_microchip(
		struct Floor * const, struct Floor * const, const int);
static struct Floor ** __copy_floors(struct Floor * const * const);
static char * __floors_to_str(struct Floor * const * const);
static bool __is_floor_safe(const struct Floor * const);
static bool __is_finished(struct Floor * const * const);
static void __enqueue(struct Queue * const, struct Floor ** const, const int);
static void __destroy_floors_and_cost(void *);

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Floor ** floors = calloc(FLOOR_COUNT, sizeof(struct Floor *));
	if (!floors) {
		printf("%s\n", strerror(errno));
		return 1;
	}

	int floor_number = 0;

	for (int i = 0; i < FLOOR_COUNT; i++) {
		struct Floor * floor = calloc(1, sizeof(struct Floor));
		if (!floor) {
			printf("%s\n", strerror(errno));
			__destroy_floors(floors);
			return 1;
		}

		floors[i] = floor;

		floor->number = floor_number;

		floor->generators = calloc(ELEMENT_COUNT, sizeof(bool));
		if (!floor->generators) {
			printf("hash_init\n");
			__destroy_floors(floors);
			return 1;
		}

		floor->microchips = calloc(ELEMENT_COUNT, sizeof(bool));
		if (!floor->microchips) {
			printf("hash_init\n");
			__destroy_floors(floors);
			return 1;
		}

		if (floor_number == 0) {
			floor->elevator = true;

#ifdef INPUT_SIMPLE
			floor->microchips[HYDROGEN] = true;
			floor->num_microchips = 1;
			floor->generators[HYDROGEN] = true;
			floor->num_generators = 1;
#endif
#ifdef INPUT_SAMPLE
			floor->microchips[HYDROGEN] = true;
			floor->microchips[LITHIUM] = true;
			floor->num_microchips = 2;
#endif
#ifdef INPUT_PART1
			floor->generators[PROMETHIUM] = true;
			floor->microchips[PROMETHIUM] = true;
			floor->num_generators = 1;
			floor->num_microchips = 1;
#endif
#ifdef INPUT_PART2
			floor->generators[PROMETHIUM] = true;
			floor->microchips[PROMETHIUM] = true;
			floor->generators[ELERIUM] = true;
			floor->microchips[ELERIUM] = true;
			floor->generators[DILITHIUM] = true;
			floor->microchips[DILITHIUM] = true;
			floor->num_generators = 3;
			floor->num_microchips = 3;
#endif
		}

		if (floor_number == 1) {
#ifdef INPUT_SAMPLE
			floor->generators[HYDROGEN] = true;
			floor->num_generators = 1;
#endif
#ifdef INPUT_PART1
			floor->generators[COBALT] = true;
			floor->generators[CURIUM] = true;
			floor->generators[RUTHENIUM] = true;
			floor->generators[PLUTONIUM] = true;
			floor->num_generators = 4;
#endif
#ifdef INPUT_PART2
			floor->generators[COBALT] = true;
			floor->generators[CURIUM] = true;
			floor->generators[RUTHENIUM] = true;
			floor->generators[PLUTONIUM] = true;
			floor->num_generators = 4;
#endif
		}

		if (floor_number == 2) {
#ifdef INPUT_SAMPLE
			floor->generators[LITHIUM] = true;
			floor->num_generators = 1;
#endif
#ifdef INPUT_PART1
			floor->microchips[COBALT] = true;
			floor->microchips[CURIUM] = true;
			floor->microchips[RUTHENIUM] = true;
			floor->microchips[PLUTONIUM] = true;
			floor->num_microchips = 4;
#endif
#ifdef INPUT_PART2
			floor->microchips[COBALT] = true;
			floor->microchips[CURIUM] = true;
			floor->microchips[RUTHENIUM] = true;
			floor->microchips[PLUTONIUM] = true;
			floor->num_microchips = 4;
#endif
		}

		floor_number++;
	}

	if (false) {
		printf("Starting state:\n");
		__print_floors(floors);
	}

	const int steps = __solve_bfs(floors);
	printf("%d\n", steps);

	__destroy_floors(floors);

	return 0;
}

static void
__destroy_floors(struct Floor ** const floors)
{
	if (!floors) {
		return;
	}

	for (int i = FLOOR_COUNT - 1; i >= 0; i--) {
		__destroy_floor(floors[i]);
	}

	if (floors) {
		free(floors);
	}
}

static void
__destroy_floor(struct Floor * const floor)
{
	if (!floor) {
		return;
	}

	if (floor->generators) {
		free(floor->generators);
	}

	if (floor->microchips) {
		free(floor->microchips);
	}

	free(floor);
}

static void
__print_floors(struct Floor * const * const floors)
{
	for (int i = FLOOR_COUNT - 1; i >= 0; i--) {
		__print_floor(floors[i]);
	}
}

static void
__print_floor(const struct Floor * const floor)
{
	printf("%d ", floor->number + 1);

	if (floor->elevator) {
		printf("E ");
	}

	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!floor->generators[i]) {
			continue;
		}

#ifdef INPUT_SIMPLE
		if (i == HYDROGEN) {
			printf("Hydrogen generator, ");
		}
#endif
#ifdef INPUT_SAMPLE
		if (i == HYDROGEN) {
			printf("Hydrogen generator, ");
		}

		if (i == LITHIUM) {
			printf("Lithium generator, ");
		}
#endif
#ifdef INPUT_PART1
		if (i == PROMETHIUM) {
			printf("Promethium generator, ");
		}
		if (i == COBALT) {
			printf("Cobalt generator, ");
		}
		if (i == CURIUM) {
			printf("Curium generator, ");
		}
		if (i == RUTHENIUM) {
			printf("Ruthenium generator, ");
		}
		if (i == PLUTONIUM) {
			printf("Plutonium generator, ");
		}
#endif
#ifdef INPUT_PART2
		if (i == PROMETHIUM) {
			printf("Promethium generator, ");
		}
		if (i == COBALT) {
			printf("Cobalt generator, ");
		}
		if (i == CURIUM) {
			printf("Curium generator, ");
		}
		if (i == RUTHENIUM) {
			printf("Ruthenium generator, ");
		}
		if (i == PLUTONIUM) {
			printf("Plutonium generator, ");
		}
		if (i == ELERIUM) {
			printf("Elerium generator, ");
		}
		if (i == DILITHIUM) {
			printf("Dilithium generator, ");
		}
#endif
	}

	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!floor->microchips[i]) {
			continue;
		}

#ifdef INPUT_SIMPLE
		if (i == HYDROGEN) {
			printf("Hydrogen microchip, ");
		}
#endif
#ifdef INPUT_SAMPLE
		if (i == HYDROGEN) {
			printf("Hydrogen microchip, ");
		}

		if (i == LITHIUM) {
			printf("Lithium microchip, ");
		}
#endif
#ifdef INPUT_PART1
		if (i == PROMETHIUM) {
			printf("Promethium microchip, ");
		}
		if (i == COBALT) {
			printf("Cobalt microchip, ");
		}
		if (i == CURIUM) {
			printf("Curium microchip, ");
		}
		if (i == RUTHENIUM) {
			printf("Ruthenium microchip, ");
		}
		if (i == PLUTONIUM) {
			printf("Plutonium microchip, ");
		}
#endif
#ifdef INPUT_PART2
		if (i == PROMETHIUM) {
			printf("Promethium microchip, ");
		}
		if (i == COBALT) {
			printf("Cobalt microchip, ");
		}
		if (i == CURIUM) {
			printf("Curium microchip, ");
		}
		if (i == RUTHENIUM) {
			printf("Ruthenium microchip, ");
		}
		if (i == PLUTONIUM) {
			printf("Plutonium microchip, ");
		}
		if (i == ELERIUM) {
			printf("Elerium microchip, ");
		}
		if (i == DILITHIUM) {
			printf("Dilithium microchip, ");
		}
#endif
	}

	printf("\n");
}

static int
__solve_bfs(struct Floor ** const floors)
{
	struct Queue * const queue = calloc(1, sizeof(struct Queue));
	assert(queue != NULL);

	__enqueue(queue, floors, 0);

	struct htable * const visited = hash_init(HASH_SIZE);
	assert(visited);

	while (queue->first) {
		struct FloorsAndCost * const fac = dequeue(queue);
		assert(fac != NULL);

		struct Floor ** const current_floors = fac->floors;

		char * const floors_str = __floors_to_str(current_floors);
		assert(floors_str != NULL);

		if (hash_has_key(visited, floors_str)) {
			__destroy_floors_and_cost(fac);
			free(floors_str);
			continue;
		}

		assert(hash_set(visited, floors_str, NULL));
		free(floors_str);

		// Is this a solution?
		if (__is_finished(current_floors)) {
			const int cost = fac->cost;
			destroy_queue(queue, __destroy_floors_and_cost);
			__destroy_floors_and_cost(fac);
			assert(hash_free(visited, NULL));
			return cost;
		}

		// Find the current floor.
		// TODO: We could keep this in FloorsAndCost.
		struct Floor * current_floor = NULL;
		for (size_t i = 0; i < FLOOR_COUNT; i++) {
			if (current_floors[i]->elevator) {
				current_floor = current_floors[i];
				break;
			}
		}
		assert(current_floor != NULL);

		// Find all valid moves at this level.

		// Up moves
		if (current_floor->number != FLOOR_COUNT - 1) {
			struct Floor * const next_floor =
					current_floors[current_floor->number + 1];
			__get_moves_bfs(
					queue, current_floors, current_floor, next_floor, fac->cost + 1);
		}

		// Down moves
		if (current_floor->number != 0) {
			struct Floor * const next_floor =
					current_floors[current_floor->number - 1];
			__get_moves_bfs(
					queue, current_floors, current_floor, next_floor, fac->cost + 1);
		}

		__destroy_floors_and_cost(fac);
	}

	destroy_queue(queue, __destroy_floors_and_cost);
	assert(hash_free(visited, NULL));
	return -1;
}

// True if making a move solves it.
static void
__get_moves_bfs(struct Queue * const queue,
		struct Floor ** const floors,
		struct Floor * const current_floor,
		struct Floor * const next_floor,
		const int cost)
{
	current_floor->elevator = false;
	next_floor->elevator = true;

	// Move one generator.
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!current_floor->generators[i]) {
			continue;
		}

		__move_generator(next_floor, current_floor, i);

		if (__is_floor_safe(current_floor) && __is_floor_safe(next_floor)) {
			__enqueue(queue, floors, cost);
		}

		__move_generator(current_floor, next_floor, i);
	}

	// Move two generators.
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!current_floor->generators[i]) {
			continue;
		}

		for (int j = i + 1; j < ELEMENT_COUNT; j++) {
			if (!current_floor->generators[j]) {
				continue;
			}

			__move_generator(next_floor, current_floor, i);
			__move_generator(next_floor, current_floor, j);

			if (__is_floor_safe(current_floor) && __is_floor_safe(next_floor)) {
				__enqueue(queue, floors, cost);
			}

			__move_generator(current_floor, next_floor, i);
			__move_generator(current_floor, next_floor, j);
		}
	}

	// Move one microchip.
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!current_floor->microchips[i]) {
			continue;
		}

		__move_microchip(next_floor, current_floor, i);

		if (__is_floor_safe(next_floor)) {
			__enqueue(queue, floors, cost);
		}

		__move_microchip(current_floor, next_floor, i);
	}

	// Move two microchips.
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!current_floor->microchips[i]) {
			continue;
		}

		for (int j = i + 1; j < ELEMENT_COUNT; j++) {
			if (!current_floor->microchips[j]) {
				continue;
			}

			__move_microchip(next_floor, current_floor, i);
			__move_microchip(next_floor, current_floor, j);

			if (__is_floor_safe(next_floor)) {
				__enqueue(queue, floors, cost);
			}

			__move_microchip(current_floor, next_floor, i);
			__move_microchip(current_floor, next_floor, j);
		}
	}

	// Move a generator and a microchip.
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if (!current_floor->generators[i]) {
			continue;
		}

		for (int j = 0; j < ELEMENT_COUNT; j++) {
			if (!current_floor->microchips[j]) {
				continue;
			}

			__move_generator(next_floor, current_floor, i);
			__move_microchip(next_floor, current_floor, j);

			if (__is_floor_safe(current_floor) && __is_floor_safe(next_floor)) {
				__enqueue(queue, floors, cost);
			}

			__move_generator(current_floor, next_floor, i);
			__move_microchip(current_floor, next_floor, j);
		}
	}

	current_floor->elevator = true;
	next_floor->elevator = false;
}

static void
__move_generator(
		struct Floor * const dest, struct Floor * const src, const int i)
{
	src->generators[i] = false;
	src->num_generators -= 1;
	dest->generators[i] = true;
	dest->num_generators += 1;
}

static void
__move_microchip(
		struct Floor * const dest, struct Floor * const src, const int i)
{
	src->microchips[i] = false;
	src->num_microchips -= 1;
	dest->microchips[i] = true;
	dest->num_microchips += 1;
}

static struct Floor **
__copy_floors(struct Floor * const * const floors)
{
	struct Floor ** const floors2 = calloc(FLOOR_COUNT, sizeof(struct Floor *));
	assert(floors2 != NULL);

	for (size_t i = 0; i < FLOOR_COUNT; i++) {
		struct Floor * const floor = floors[i];
		struct Floor * const floor2 = calloc(1, sizeof(struct Floor));
		assert(floor2 != NULL);
		floors2[i] = floor2;

		floor2->number = floor->number;
		floor2->elevator = floor->elevator;

		floor2->generators = calloc(ELEMENT_COUNT, sizeof(bool));
		assert(floor2->generators != NULL);

		floor2->microchips = calloc(ELEMENT_COUNT, sizeof(bool));
		assert(floor2->microchips != NULL);

		for (size_t j = 0; j < ELEMENT_COUNT; j++) {
			floor2->generators[j] = floor->generators[j];
			floor2->microchips[j] = floor->microchips[j];
		}

		floor2->num_generators = floor->num_generators;
		floor2->num_microchips = floor->num_microchips;
	}

	return floors2;
}

static char *
__floors_to_str(struct Floor * const * const floors)
{
	const size_t sz = 1024;
	char * const s = calloc(1024, sizeof(char));
	if (!s) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	char buf[3];

	for (size_t i = 0; i < FLOOR_COUNT; i++) {
		const struct Floor * const floor = floors[i];

		memset(buf, 0, 3);
		snprintf(buf, 3, "F%d", floor->number);
		strncat(s, buf, sz);

		if (floor->elevator) {
			strncat(s, "E", sz);
		}

		for (size_t j = 0; j < ELEMENT_COUNT; j++) {
			if (floor->generators[j]) {
				strncat(s, "1G", sz);
			} else {
				strncat(s, "0G", sz);
			}
		}

		for (size_t j = 0; j < ELEMENT_COUNT; j++) {
			if (floor->microchips[j]) {
				strncat(s, "1M", sz);
			} else {
				strncat(s, "0M", sz);
			}
		}
	}

	return s;
}

__attribute__((pure)) static bool
__is_floor_safe(const struct Floor * const floor)
{
	if (floor->num_generators == 0) {
		return true;
	}

	if (floor->num_microchips == 0) {
		return true;
	}

	// There is at least one generator and one microchip. Each microchip must be
	// with its generator.
	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		if (!floor->microchips[i]) {
			continue;
		}

		if (!floor->generators[i]) {
			return false;
		}
	}

	return true;
}

// We are finished if everything is on the top floor.
static bool
__is_finished(struct Floor * const * const floors)
{
	for (size_t i = 0; i < FLOOR_COUNT; i++) {
		bool is_top_floor = i == FLOOR_COUNT - 1;
		if (is_top_floor) {
			continue;
		}

		const struct Floor * const floor = floors[i];

		if (floor->elevator) {
			return false;
		}

		if (floor->num_generators > 0 || floor->num_microchips > 0) {
			return false;
		}
	}

	return true;
}

static void
__enqueue(
		struct Queue * const queue, struct Floor ** const floors, const int cost)
{
	struct FloorsAndCost * fac = calloc(1, sizeof(struct FloorsAndCost));
	assert(fac != NULL);

	fac->floors = __copy_floors(floors);
	assert(fac->floors != NULL);

	fac->cost = cost;

	assert(enqueue(queue, fac));
}

static void
__destroy_floors_and_cost(void * p)
{
	if (!p) {
		return;
	}

	struct FloorsAndCost * fac = p;

	if (fac->floors) {
		__destroy_floors(fac->floors);
	}

	free(fac);
}
