#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Facing { Up, Down, Left, Right };

enum NextMove { TurnLeft, Straight, TurnRight };

struct Cart {
	enum Facing facing;
	enum NextMove next_move;
	int id;
	uint64_t ticks;
};

enum RailType { UpDown, LeftRight, CurveRight, CurveLeft, Intersection };

struct Rail {
	enum RailType type;
};

struct Position {
	struct Cart * cart;
	struct Rail * rail;
};

static void
print_cart(struct Cart const * const);

static void
print_rail(struct Rail const * const);

static void
face_cart(struct Cart * const,
		struct Rail const * const);

#define SZ 150

//#undef SZ
//#define SZ 15

#define SZ_RAILS 40960

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Cart carts[128] = {0};
	size_t n_carts = 0;
	struct Rail rails[SZ_RAILS] = {0};
	size_t n_rails = 0;
	struct Position map[SZ][SZ] = {0};
	int x = 0;
	int y = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;

		x = 0;
		while (*ptr != '\n') {
			struct Cart * cart = NULL;
			struct Rail * rail = NULL;
			if (*ptr == '|' || *ptr == '-' || *ptr == '/' || *ptr == '\\' ||
					*ptr == '+') {
				switch (*ptr) {
				case '|':
					rails[n_rails].type = UpDown;
					break;
				case '-':
					rails[n_rails].type = LeftRight;
					break;
				case '/':
					rails[n_rails].type = CurveRight;
					break;
				case '\\':
					rails[n_rails].type = CurveLeft;
					break;
				case '+':
					rails[n_rails].type = Intersection;
					break;
				default:
					assert(1 == 0);
					break;
				}
				rail = &rails[n_rails];
				n_rails++;
				assert(n_rails != SZ_RAILS);
			} else if (*ptr == '^' || *ptr == 'v' || *ptr == '<' ||
					*ptr == '>') {
				switch (*ptr) {
				case '^':
					carts[n_carts].facing = Up;
					rails[n_rails].type = UpDown;
					break;
				case 'v':
					carts[n_carts].facing = Down;
					rails[n_rails].type = UpDown;
					break;
				case '<':
					carts[n_carts].facing = Left;
					rails[n_rails].type = LeftRight;
					break;
				case '>':
					carts[n_carts].facing = Right;
					rails[n_rails].type = LeftRight;
					break;
				default:
					assert(1 == 0);
					break;
				}
				carts[n_carts].next_move = TurnLeft;
				carts[n_carts].id = (int) n_carts;
				cart = &carts[n_carts];
				n_carts++;

				rail = &rails[n_rails];
				n_rails++;
			} else if (*ptr == ' ') {
			} else {
				fprintf(stderr, "unexpected character [%c]\n", *ptr);
				return 1;
			}
			map[x][y].cart = cart;
			map[x][y].rail = rail;
			x++;
			ptr++;
		}
		y++;
	}

	uint64_t ticks = 0;
	while (1) {
		if (false) {
			for (y = 0; y < SZ; y++) {
				for (x = 0; x < SZ; x++) {
					if (map[x][y].cart) {
						print_cart(map[x][y].cart);
						continue;
					}
					if (map[x][y].rail) {
						print_rail(map[x][y].rail);
						continue;
					}
					printf(" ");
				}
				printf("\n");
			}
		}

		for (y = 0; y < SZ; y++) {
			for (x = 0; x < SZ; x++) {
				struct Cart * const cart = map[x][y].cart;
				if (!cart || cart->ticks == ticks) {
					continue;
				}

				int new_x = x, new_y = y;
				if (cart->facing == Up) {
					new_y--;
				} else if (cart->facing == Down) {
					new_y++;
				} else if (cart->facing == Left) {
					new_x--;
				} else if (cart->facing == Right) {
					new_x++;
				} else {
					assert(1 == 0);
				}

				if (new_x < 0 || new_x == SZ || new_y < 0 || new_y == SZ) {
					fprintf(stderr, "cart leaves the world\n");
					return 1;
				}
				if (false) {
					printf("moving cart %d from %d,%d to %d,%d\n", cart->id, x, y,
							new_x, new_y);
				}

				if (map[new_x][new_y].cart) {
					printf("%d,%d\n", new_x, new_y);
					return 0;
				}

				map[new_x][new_y].cart = cart;
				map[x][y].cart = NULL;
				cart->ticks = ticks;

				struct Rail const * const rail = map[new_x][new_y].rail;
				if (!rail) {
					fprintf(stderr, "no rail at new position\n");
					return 1;
				}
				face_cart(cart, rail);
			}
		}
		ticks++;
	}

	return 0;
}

static void
print_cart(struct Cart const * const cart)
{
	if (cart->facing == Up) {
		printf("^");
		return;
	}
	if (cart->facing == Down) {
		printf("v");
		return;
	}
	if (cart->facing == Left) {
		printf("<");
		return;
	}
	if (cart->facing == Right) {
		printf(">");
		return;
	}
	assert(1 == 0);
}

static void
print_rail(struct Rail const * const rail)
{
	if (rail->type == UpDown) {
		printf("|");
		return;
	}
	if (rail->type == LeftRight) {
		printf("-");
		return;
	}
	if (rail->type == CurveRight) {
		printf("/");
		return;
	}
	if (rail->type == CurveLeft) {
		printf("\\");
		return;
	}
	if (rail->type == Intersection) {
		printf("+");
		return;
	}
	assert(1 == 0);
}

static void
face_cart(struct Cart * const cart,
		struct Rail const * const rail)
{
	if (rail->type == UpDown) {
		assert(cart->facing == Up || cart->facing == Down);
		return;
	}

	if (rail->type == LeftRight) {
		assert(cart->facing == Left || cart->facing == Right);
		return;
	}

	if (rail->type == CurveRight) {
		if (cart->facing == Up) {
			cart->facing = Right;
			return;
		}
		if (cart->facing == Down) {
			cart->facing = Left;
			return;
		}
		if (cart->facing == Left) {
			cart->facing = Down;
			return;
		}
		if (cart->facing == Right) {
			cart->facing = Up;
			return;
		}
		assert(1 == 0);
	}

	if (rail->type == CurveLeft) {
		if (cart->facing == Up) {
			cart->facing = Left;
			return;
		}
		if (cart->facing == Down) {
			cart->facing = Right;
			return;
		}
		if (cart->facing == Left) {
			cart->facing = Up;
			return;
		}
		if (cart->facing == Right) {
			cart->facing = Down;
			return;
		}
		assert(1 == 0);
	}

	if (rail->type == Intersection) {
		if (cart->next_move == TurnLeft) {
			cart->next_move = Straight;
			if (cart->facing == Up) {
				cart->facing = Left;
				return;
			}
			if (cart->facing == Down) {
				cart->facing = Right;
				return;
			}
			if (cart->facing == Left) {
				cart->facing = Down;
				return;
			}
			if (cart->facing == Right) {
				cart->facing = Up;
				return;
			}
			assert(1 == 0);
		}
		if (cart->next_move == Straight) {
			cart->next_move = TurnRight;
			return;
		}
		if (cart->next_move == TurnRight) {
			cart->next_move = TurnLeft;
			if (cart->facing == Up) {
				cart->facing = Right;
				return;
			}
			if (cart->facing == Down) {
				cart->facing = Left;
				return;
			}
			if (cart->facing == Left) {
				cart->facing = Up;
				return;
			}
			if (cart->facing == Right) {
				cart->facing = Down;
				return;
			}
			assert(1 == 0);
		}
	}

	assert(1 == 0);
}
