#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Square;

enum CreatureType { Elf, Goblin };

struct Creature {
	enum CreatureType type;
	int hp;
	int id;
	struct Square * square;
	int x;
	int y;
};

enum SquareType { Wall, Cavern };

enum Direction { Unknown, Up, Right, Down, Left };

struct Square {
	enum SquareType type;
	int x;
	int y;
	int steps;
	struct Creature * creature;
	enum Direction direction;
};

static bool
run_simulation(struct Square * * const,
		struct Creature const * const,
		size_t const,
		int const);

static void
find_targets(struct Creature const * const,
		size_t const,
		struct Creature const * const,
		struct Creature * const,
		size_t * const);

static void
print_answer(struct Creature const * const,
		size_t const,
		uint64_t const);

static void
take_turn(struct Square * * const,
		struct Creature * const,
		struct Creature * const,
		size_t const,
		int const);

static enum Direction
choose_direction(struct Square * * const,
		struct Creature const * const,
		struct Creature const * const,
		size_t const);

static void
find_in_range_squares(struct Square * * const,
		struct Creature const * const,
		size_t const,
		struct Square * const,
		size_t * const);

static struct Square *
shortest_path(struct Square * * const,
		int const,
		int const,
		int const,
		int const);

static void
enqueue_square(struct Queue * const,
		int const,
		int const,
		int const,
		enum Direction const);

static void
add_neighbours(struct Square * * const,
		struct Queue * const,
		struct Square const * const);

static bool
square_is_open(struct Square * * const,
		int const,
		int const);

static int
cmp_squares(void const * const, void const * const);

static bool
maybe_attack(struct Square * * const,
		struct Creature const * const,
		int const);

static int
cmp_creatures(void const * const, void const * const);

// XXX
#define MAP_SZ 32
//#define MAP_SZ 7
#define SQUARES_SZ 1024
#define CREATURES_SZ 64

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Creature creatures[CREATURES_SZ] = {0};
	size_t n_creatures = 0;
	struct Square * * const map = calloc(MAP_SZ, sizeof(struct Square *));
	assert(map != NULL);
	for (size_t i = 0; i < MAP_SZ; i++) {
		map[i] = calloc(MAP_SZ, sizeof(struct Square));
		assert(map[i] != NULL);
	}
	int x = 0;
	int y = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;
		x = 0;
		while (*ptr != '\0') {
			if (*ptr == '#') {
				map[x][y].type = Wall;
			} else if (*ptr == '.') {
				map[x][y].type = Cavern;
			} else if (*ptr == 'G' || *ptr == 'E') {
				creatures[n_creatures].type = *ptr == 'G' ? Goblin : Elf;
				creatures[n_creatures].hp = 200;
				creatures[n_creatures].id = (int) n_creatures;
				creatures[n_creatures].square = &map[x][y];
				creatures[n_creatures].x = x;
				creatures[n_creatures].y = y;
				map[x][y].type = Cavern;
				map[x][y].creature = &creatures[n_creatures];
				n_creatures++;
				assert(n_creatures <= CREATURES_SZ);
			} else {
				assert(1 == 0);
			}
			map[x][y].x = x;
			map[x][y].y = y;
			x++;
			ptr++;
		}
		y++;
	}

	int attack_power = 4;
	while (1) {
		if (!run_simulation(map, creatures, n_creatures, attack_power)) {
			attack_power++;
			continue;
		}
		break;
	}
	printf("%d\n", attack_power);
	for (size_t i = 0; i < MAP_SZ; i++) {
		free(map[i]);
	}
	free(map);

	return 0;
}

static bool
run_simulation(struct Square * * const map,
		struct Creature const * const creatures,
		size_t const n_creatures,
		int const attack_power)
{
	int elves_before = 0;
	for (size_t i = 0; i < n_creatures; i++) {
		if (creatures[i].type == Elf) {
			elves_before++;
		}
	}

	struct Square * * const map2 = calloc(MAP_SZ, sizeof(struct Square *));
	assert(map2 != NULL);
	for (size_t i = 0; i < MAP_SZ; i++) {
		map2[i] = calloc(MAP_SZ, sizeof(struct Square));
		assert(map2[i] != NULL);
		memcpy(map2[i], map[i], MAP_SZ*sizeof(struct Square));
	}

	struct Creature creatures2[CREATURES_SZ] = {0};
	memcpy(creatures2, creatures, CREATURES_SZ*sizeof(struct Creature));

	for (size_t i = 0; i < n_creatures; i++) {
		struct Creature * const creature = &creatures2[i];
		int const x = creature->x;
		int const y = creature->y;
		creature->square = &map2[x][y];
		map2[x][y].creature = creature;
	}

	uint64_t rounds = 0;
	while (1) {
		int moved[CREATURES_SZ] = {0};
		for (int y = 0; y < MAP_SZ; y++) {
			for (int x = 0; x < MAP_SZ; x++) {
				struct Creature * const me = map2[x][y].creature;
				if (!me || moved[me->id]) {
					continue;
				}

				struct Creature targets[CREATURES_SZ] = {0};
				size_t n_targets = 0;
				find_targets(creatures2, n_creatures, me, targets, &n_targets);
				if (n_targets == 0) {
					goto done;
				}

				take_turn(map2, me, targets, n_targets, attack_power);
				moved[me->id] = 1;
			}
		}
		rounds++;
	}

done:
	for (size_t i = 0; i < MAP_SZ; i++) {
		free(map2[i]);
	}
	free(map2);

	int elves_after = 0;
	int goblins_after = 0;
	for (size_t i = 0; i < n_creatures; i++) {
		if (creatures2[i].hp <= 0) {
			continue;
		}
		if (creatures2[i].type == Goblin) {
			goblins_after++;
			continue;
		}
		if (creatures2[i].type == Elf) {
			elves_after++;
			continue;
		}
		assert(1 == 0);
	}

	bool const found = goblins_after == 0 && elves_after == elves_before;
	if (found) {
		print_answer(creatures2, n_creatures, rounds);
	}
	return found;
}

static void
find_targets(struct Creature const * const creatures,
		size_t const n_creatures,
		struct Creature const * const me,
		struct Creature * const targets,
		size_t * const n_targets)
{
	for (size_t i = 0; i < n_creatures; i++) {
		struct Creature const * const other = &creatures[i];
		if (other->hp <= 0 || other == me || other->type == me->type) {
			continue;
		}
		targets[*n_targets] = creatures[i];
		*n_targets += 1;
		assert(*n_targets <= CREATURES_SZ);
	}
}

static void
print_answer(struct Creature const * const creatures,
		size_t const n_creatures,
		uint64_t const rounds)
{
	int hp = 0;
	for (size_t i = 0; i < n_creatures; i++) {
		struct Creature const * const creature = &creatures[i];
		if (creature->hp <= 0) {
			continue;
		}
		hp += creature->hp;
	}

	printf("%" PRIu64 "\n", rounds*(uint64_t) hp);
}

static void
take_turn(struct Square * * const map,
		struct Creature * const me,
		struct Creature * const targets,
		size_t const n_targets,
		int const attack_power)
{
	if (maybe_attack(map, me, attack_power)) {
		return;
	}

	enum Direction const direction = choose_direction(map, me, targets,
			n_targets);
	if (direction == Unknown) {
		return;
	}

	struct Square * next_square = NULL;
	if (direction == Up) {
		next_square = &map[me->square->x][me->square->y-1];
	} else if (direction == Right) {
		next_square = &map[me->square->x+1][me->square->y];
	} else if (direction == Down) {
		next_square = &map[me->square->x][me->square->y+1];
	} else if (direction == Left) {
		next_square = &map[me->square->x-1][me->square->y];
	} else {
		assert(1 == 0);
	}

	assert(next_square->creature == NULL);

	me->square->creature = NULL;
	next_square->creature = me;
	me->square = next_square;

	maybe_attack(map, me, attack_power);
}

static enum Direction
choose_direction(struct Square * * const map,
		struct Creature const * const me,
		struct Creature const * const targets,
		size_t const n_targets)
{
	struct Square in_range_squares[SQUARES_SZ] = {0};
	size_t n_in_range_squares = 0;
	find_in_range_squares(map, targets, n_targets, in_range_squares,
			&n_in_range_squares);
	if (n_in_range_squares == 0) {
		return Unknown;
	}

	struct Square reachable_squares[SQUARES_SZ] = {0};
	size_t n_reachable_squares = 0;
	for (size_t i = 0; i < n_in_range_squares; i++) {
		struct Square * const square = shortest_path(map, me->square->x,
				me->square->y, in_range_squares[i].x, in_range_squares[i].y);
		if (!square) {
			continue;
		}
		reachable_squares[n_reachable_squares++] = *square;
		free(square);
	}

	if (n_reachable_squares == 0) {
		return Unknown;
	}

	qsort(reachable_squares, n_reachable_squares, sizeof(struct Square),
			cmp_squares);

	return reachable_squares[0].direction;
}

static void
find_in_range_squares(struct Square * * const map,
		struct Creature const * const targets,
		size_t const n_targets,
		struct Square * const squares,
		size_t * const n_squares)
{
	for (size_t i = 0; i < n_targets; i++) {
		struct Creature const * const other = &targets[i];

		int x = other->square->x;
		int y = other->square->y-1;
		if (square_is_open(map, x, y)) {
			squares[*n_squares].x = x;
			squares[*n_squares].y = y;
			*n_squares += 1;
			assert(*n_squares <= SQUARES_SZ);
		}

		x = other->square->x+1;
		y = other->square->y;
		if (square_is_open(map, x, y)) {
			squares[*n_squares].x = x;
			squares[*n_squares].y = y;
			*n_squares += 1;
			assert(*n_squares <= SQUARES_SZ);
		}

		x = other->square->x;
		y = other->square->y+1;
		if (square_is_open(map, x, y)) {
			squares[*n_squares].x = x;
			squares[*n_squares].y = y;
			*n_squares += 1;
			assert(*n_squares <= SQUARES_SZ);
		}

		x = other->square->x-1;
		y = other->square->y;
		if (square_is_open(map, x, y)) {
			squares[*n_squares].x = x;
			squares[*n_squares].y = y;
			*n_squares += 1;
			assert(*n_squares <= SQUARES_SZ);
		}
	}
}

static struct Square *
shortest_path(struct Square * * const map,
		int const x_0,
		int const y_0,
		int const x_1,
		int const y_1)
{
	struct Queue * const queue = calloc(1, sizeof(struct Queue));
	assert(queue != NULL);

	enqueue_square(queue, x_0, y_0, 0, Unknown);

	struct htable * const visited = hash_init(1024);
	assert(visited != NULL);

#define CHOICES_SZ 1024
	struct Square choices[CHOICES_SZ] = {0};
	size_t n_choices = 0;
	int shortest_so_far = -1;

	while (queue->first) {
		struct Square * const square = dequeue(queue);
		assert(square != NULL);

		if (square->x == x_1 && square->y == y_1) {
			if (shortest_so_far == -1) {
				shortest_so_far = square->steps;
			}
			if (square->steps > shortest_so_far) {
				free(square);
				continue;
			}
			if (square->steps == shortest_so_far) {
				choices[n_choices++] = *square;
				assert(n_choices <= CHOICES_SZ);
				free(square);
				continue;
			}
			n_choices = 0;
			choices[n_choices++] = *square;
			assert(n_choices <= CHOICES_SZ);
			shortest_so_far = square->steps;
			free(square);
			continue;
		}

		if (shortest_so_far != -1 && square->steps+1 > shortest_so_far) {
			free(square);
			continue;
		}

		//if (square->steps > 20) {
		//if (square->steps > 10) {
		////if (square->steps > MAP_SZ) {
		//	free(square);
		//	continue;
		//}

		char buf[32] = {0};
		assert(sprintf(buf, "%d,%d", square->x, square->y) >= 0);
		if (hash_has_key(visited, buf)) {
			free(square);
			continue;
		}
		assert(hash_set(visited, buf, NULL));

		add_neighbours(map, queue, square);
		free(square);
	}

	destroy_queue(queue, free);
	assert(hash_free(visited, NULL));

	if (n_choices == 0) {
		return NULL;
	}

	struct Square move_squares[CHOICES_SZ] = {0};
	for (size_t i = 0; i < n_choices; i++) {
		move_squares[i] = choices[i];
		move_squares[i].x = x_0;
		move_squares[i].y = y_0;
		if (choices[i].direction == Up) {
			move_squares[i].y--;
		} else if (choices[i].direction == Right) {
			move_squares[i].x++;
		} else if (choices[i].direction == Down) {
			move_squares[i].y++;
		} else if (choices[i].direction == Left) {
			move_squares[i].x--;
		} else {
			assert(1 == 0);
		}
	}

	qsort(move_squares, n_choices, sizeof(struct Square), cmp_squares);

	struct Square * const square = calloc(1, sizeof(struct Square));
	assert(square != NULL);
	memcpy(square, &move_squares[0], sizeof(struct Square));

	return square;
}

static void
enqueue_square(struct Queue * const queue,
		int const x,
		int const y,
		int const steps,
		enum Direction const direction)
{
	struct Square * const square = calloc(1, sizeof(struct Square));
	assert(square != NULL);
	square->x = x;
	square->y = y;
	square->steps = steps;
	square->direction = direction;
	assert(enqueue(queue, square));
}

static void
add_neighbours(struct Square * * const map,
		struct Queue * const queue,
		struct Square const * const from)
{
	int new_x = from->x;
	int new_y = from->y-1;
	if (square_is_open(map, new_x, new_y)) {
		enum Direction moving = from->direction;
		if (moving == Unknown) {
			moving = Up;
		}
		enqueue_square(queue, new_x, new_y, from->steps+1, moving);
	}

	new_x = from->x-1;
	new_y = from->y;
	if (square_is_open(map, new_x, new_y)) {
		enum Direction moving = from->direction;
		if (moving == Unknown) {
			moving = Left;
		}
		enqueue_square(queue, new_x, new_y, from->steps+1, moving);
	}

	new_x = from->x+1;
	new_y = from->y;
	if (square_is_open(map, new_x, new_y)) {
		enum Direction moving = from->direction;
		if (moving == Unknown) {
			moving = Right;
		}
		enqueue_square(queue, new_x, new_y, from->steps+1, moving);
	}

	new_x = from->x;
	new_y = from->y+1;
	if (square_is_open(map, new_x, new_y)) {
		enum Direction moving = from->direction;
		if (moving == Unknown) {
			moving = Down;
		}
		enqueue_square(queue, new_x, new_y, from->steps+1, moving);
	}
}

__attribute__((pure))
static bool
square_is_open(struct Square * * const map,
		int const x,
		int const y)
{
	if (x < 0 || x == MAP_SZ ||
			y < 0 || y == MAP_SZ) {
		return false;
	}
	if (map[x][y].type == Wall) {
		return false;
	}
	if (map[x][y].creature) {
		return false;
	}
	return true;
}

__attribute__((pure))
static int
cmp_squares(void const * const a_v, void const * const b_v)
{
	struct Square const * const a = a_v;
	struct Square const * const b = b_v;

	if (a->steps < b->steps) {
		return -1;
	}
	if (a->steps > b->steps) {
		return 1;
	}
	if (a->y < b->y) {
		return -1;
	}
	if (a->y > b->y) {
		return 1;
	}
	if (a->x < b->x) {
		return -1;
	}
	if (a->x > b->x) {
		return 1;
	}
	return 0;
}

static bool
maybe_attack(struct Square * * const map,
		struct Creature const * const me,
		int const attack_power)
{
	struct Creature * targets[4] = {0};
	size_t n_targets = 0;

	int x = me->square->x;
	int y = me->square->y-1;
	if (y != -1 &&
			map[x][y].creature &&
			map[x][y].creature->type != me->type) {
		targets[n_targets++] = map[x][y].creature;
	}

	x = me->square->x+1;
	y = me->square->y;
	if (x != MAP_SZ &&
			map[x][y].creature &&
			map[x][y].creature->type != me->type) {
		targets[n_targets++] = map[x][y].creature;
	}

	x = me->square->x;
	y = me->square->y+1;
	if (y != MAP_SZ &&
			map[x][y].creature &&
			map[x][y].creature->type != me->type) {
		targets[n_targets++] = map[x][y].creature;
	}

	x = me->square->x-1;
	y = me->square->y;
	if (x != -1 &&
			map[x][y].creature &&
			map[x][y].creature->type != me->type) {
		targets[n_targets++] = map[x][y].creature;
	}

	if (n_targets == 0) {
		return false;
	}

	qsort(targets, n_targets, sizeof(struct Creature *), cmp_creatures);

	struct Creature * const target = targets[0];

	if (me->type == Goblin) {
		target->hp -= 3;
	} else {
		target->hp -= attack_power;
	}
	if (target->hp <= 0) {
		target->square->creature = NULL;
	}

	return true;
}

__attribute__((pure))
static int
cmp_creatures(void const * const a_v, void const * const b_v)
{
	struct Creature * const * const aa = a_v;
	struct Creature * const * const bb = b_v;
	struct Creature const * const a = *aa;
	struct Creature const * const b = *bb;

	if (a->hp < b->hp) {
		return -1;
	}
	if (a->hp > b->hp) {
		return 1;
	}
	if (a->square->y < b->square->y) {
		return -1;
	}
	if (a->square->y > b->square->y) {
		return 1;
	}
	if (a->square->x < b->square->x) {
		return 1;
	}
	if (a->square->x > b->square->x) {
		return 1;
	}
	return 0;
}
