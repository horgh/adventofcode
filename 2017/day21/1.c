#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 1024
#define MAX_SIZE 4096
#define MAX_INPUT_PATTERNS 128

#define DEBUG 0

struct grid {
	size_t capacity;
	size_t size;
	bool ** pixels;
};

struct rule {
	struct grid ** inputs;
	size_t num_inputs;
	struct grid * output;
};

static void destroy_rules(struct rule ** const);
static void destroy_rule(struct rule * const);
static void destroy_grid(struct grid * const);
static struct rule * parse_line(char const * const);
static struct grid * new_grid(size_t const);
static char const * parse_grid(struct grid * const, char const * const);
static char const * parse_row(
		struct grid * const, size_t const, char const * const);
static bool add_inputs(struct rule * const, struct grid const * const);
static struct grid * copy_grid(struct grid const * const);
static void transpose_grid(
		struct grid * const, struct grid const * const, size_t const, size_t const);
static void flip_vertical(struct grid * const);
static void swap_rows(struct grid * const, size_t const, size_t const);
static void flip_horizontal(struct grid * const);
static void swap_columns(struct grid * const, size_t const, size_t const);
static void rotate(struct grid * const);
static void move_pixel(struct grid * const,
		struct grid const * const,
		size_t const,
		size_t const,
		size_t const,
		size_t const);
static bool rule_matches_grid(
		struct rule const * const, struct grid const * const);
static bool grids_match(struct grid const * const, struct grid const * const);
static void add_input_to_rule(struct rule * const, struct grid const * const);
static void print_rule(struct rule const * const);
static void print_grid(struct grid const * const);
static bool run(
		struct rule ** const, size_t const, struct grid * const, size_t const);
static struct grid * apply_rules(struct rule ** const,
		size_t const,
		struct grid const * const,
		size_t const);
static struct grid * match(struct rule ** const,
		size_t const,
		struct grid const * const,
		size_t const,
		size_t const,
		size_t const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <steps>\n", argv[0]);
		return 1;
	}
	size_t const steps = (size_t)atoi(argv[1]);

	FILE * const fh = stdin;
	char buf[4096] = {0};

	struct rule * rules[MAX_RULES] = {0};
	size_t num_rules = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				destroy_rules(rules);
				return 1;
			}
			break;
		}

		char * ptr = buf;
		while (*ptr != '\0') {
			if (*ptr == '\n') {
				*ptr = '\0';
				break;
			}
			ptr++;
		}

		rules[num_rules] = parse_line(buf);
		if (!rules[num_rules]) {
			fprintf(stderr, "parse_line()\n");
			destroy_rules(rules);
			return 1;
		}

		num_rules++;

		if (num_rules == MAX_RULES) {
			fprintf(stderr, "hit max rules\n");
			destroy_rules(rules);
			return 1;
		}
	}

	if (DEBUG) {
		for (size_t i = 0; i < num_rules; i++) {
			print_rule(rules[i]);
		}
	}

	struct grid * const g = new_grid(MAX_SIZE);
	if (!g) {
		fprintf(stderr, "new_grid()\n");
		destroy_rules(rules);
		return 1;
	}

	g->size = 3;
	g->pixels[0][1] = true;
	g->pixels[1][2] = true;
	g->pixels[2][0] = true;
	g->pixels[2][1] = true;
	g->pixels[2][2] = true;

	if (!run(rules, num_rules, g, steps)) {
		fprintf(stderr, "run()\n");
		destroy_rules(rules);
		destroy_grid(g);
		return 1;
	}

	destroy_rules(rules);
	destroy_grid(g);

	return 0;
}

static void
destroy_rules(struct rule ** const rules)
{
	if (!rules) {
		return;
	}

	for (size_t i = 0;; i++) {
		if (!rules[i]) {
			break;
		}

		destroy_rule(rules[i]);
	}
}

static void
destroy_rule(struct rule * const rule)
{
	if (!rule) {
		return;
	}

	if (rule->inputs) {
		for (size_t i = 0; i < rule->num_inputs; i++) {
			destroy_grid(rule->inputs[i]);
		}
		free(rule->inputs);
	}

	if (rule->output) {
		destroy_grid(rule->output);
	}

	free(rule);
}

static void
destroy_grid(struct grid * const grid)
{
	if (!grid) {
		return;
	}

	if (grid->pixels) {
		for (size_t i = 0; i < grid->capacity; i++) {
			free(grid->pixels[i]);
		}
		free(grid->pixels);
	}

	free(grid);
}

static struct rule *
parse_line(char const * const s)
{
	struct rule * const r = calloc(1, sizeof(struct rule));
	if (!r) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = s;

	r->inputs = calloc(MAX_INPUT_PATTERNS, sizeof(struct grid *));
	if (!r->inputs) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		destroy_rule(r);
		return NULL;
	}

	struct grid * const input_pattern = new_grid(4);
	if (!input_pattern) {
		fprintf(stderr, "new_grid()\n");
		destroy_rule(r);
		return NULL;
	}

	ptr = parse_grid(input_pattern, ptr);
	if (!ptr) {
		fprintf(stderr, "parse_grid()\n");
		destroy_rule(r);
		destroy_grid(input_pattern);
		return NULL;
	}

	if (!add_inputs(r, input_pattern)) {
		fprintf(stderr, "parse_grid()\n");
		destroy_rule(r);
		destroy_grid(input_pattern);
		return NULL;
	}
	destroy_grid(input_pattern);

	if (strncmp(ptr, " => ", 4) != 0) {
		fprintf(stderr, "expecting ' => ': %s\n", ptr);
		destroy_rule(r);
		return NULL;
	}
	ptr += 4;

	r->output = new_grid(4);
	if (!r->output) {
		fprintf(stderr, "new_grid(): %s\n", strerror(errno));
		destroy_rule(r);
		return NULL;
	}

	ptr = parse_grid(r->output, ptr);
	if (!ptr) {
		fprintf(stderr, "parse_grid()\n");
		destroy_rule(r);
		return NULL;
	}

	return r;
}

static struct grid *
new_grid(size_t const capacity)
{
	struct grid * const g = calloc(1, sizeof(struct grid));
	if (!g) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	g->capacity = capacity;

	g->pixels = calloc(capacity, sizeof(bool *));
	if (!g->pixels) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		destroy_grid(g);
		return NULL;
	}

	for (size_t i = 0; i < capacity; i++) {
		g->pixels[i] = calloc(capacity, sizeof(bool));
		if (!g->pixels[i]) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			destroy_grid(g);
			return NULL;
		}
	}

	return g;
}

static char const *
parse_grid(struct grid * const g, char const * const s)
{
	char const * ptr = s;

	size_t row = 0;

	while (1) {
		ptr = parse_row(g, row, ptr);
		if (!ptr) {
			fprintf(stderr, "parse_row()\n");
			return NULL;
		}

		row++;

		if (*ptr == '\0' || *ptr == ' ') {
			break;
		}

		if (*ptr == '/') {
			ptr++;
			continue;
		}

		fprintf(stderr, "%s: unexpected position: %s\n", __func__, ptr);
		return NULL;
	}

	g->size = row;
	return ptr;
}

static char const *
parse_row(struct grid * const g, size_t const row, char const * const s)
{
	char const * ptr = s;

	if (*ptr != '#' && *ptr != '.') {
		fprintf(stderr, "%s: expected '#' or '.': %s\n", __func__, ptr);
		return NULL;
	}

	size_t col = 0;

	while (*ptr != '\0' && *ptr != '/' && *ptr != ' ') {
		if (*ptr == '#') {
			g->pixels[row][col] = true;
			col++;
			ptr++;
			continue;
		}

		if (*ptr == '.') {
			col++;
			ptr++;
			continue;
		}

		fprintf(stderr, "unexpected character: %s\n", ptr);
		return NULL;
	}

	return ptr;
}

static bool
add_inputs(struct rule * const r, struct grid const * const input_pattern)
{
	struct grid * const g = copy_grid(input_pattern);
	if (!g) {
		fprintf(stderr, "copy_grid()\n");
		return false;
	}

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 2; j++) {
			if (DEBUG) {
				printf("grid before flipping vertically:\n");
				print_grid(g);
			}

			flip_vertical(g);

			if (DEBUG) {
				printf(
						"grid after flipping vertically, before flipping horizontally:\n");
				print_grid(g);
			}

			if (!rule_matches_grid(r, g)) {
				add_input_to_rule(r, g);
			}

			flip_horizontal(g);

			if (!rule_matches_grid(r, g)) {
				add_input_to_rule(r, g);
			}

			if (DEBUG) {
				printf("grid after flipping horizontally:\n");
				print_grid(g);
			}
		}

		if (DEBUG) {
			printf("grid before rotating:\n");
			print_grid(g);
		}
		rotate(g);
		if (DEBUG) {
			printf("grid after rotating:\n");
			print_grid(g);
		}
	}

	destroy_grid(g);
	return true;
}

static struct grid *
copy_grid(struct grid const * const g)
{
	struct grid * const g2 = new_grid(g->capacity);
	if (!g2) {
		fprintf(stderr, "new_grid()\n");
		return NULL;
	}

	g2->size = g->size;

	transpose_grid(g2, g, 0, 0);

	return g2;
}

static void
transpose_grid(struct grid * const target,
		struct grid const * const source,
		size_t const dest_row,
		size_t const dest_col)
{
	size_t target_row = dest_row;
	size_t target_col = dest_col;
	for (size_t row = 0; row < source->size; row++) {
		for (size_t col = 0; col < source->size; col++) {
			// TODO: I do no bounds checking! Raaaggh!!
			target->pixels[target_row + row][target_col + col] =
					source->pixels[row][col];
		}
	}
}

static void
flip_vertical(struct grid * const g)
{
	if (g->size == 2) {
		swap_rows(g, 0, 1);
		return;
	}

	if (g->size == 3) {
		swap_rows(g, 0, 2);
		return;
	}

	if (g->size == 4) {
		swap_rows(g, 0, 3);
		swap_rows(g, 1, 2);
		return;
	}

	fprintf(stderr, "%s: unhandled size: %zu\n", __func__, g->size);
}

static void
swap_rows(struct grid * const g, size_t const a, size_t const b)
{
	for (size_t i = 0; i < g->size; i++) {
		bool const tmp = g->pixels[a][i];
		g->pixels[a][i] = g->pixels[b][i];
		g->pixels[b][i] = tmp;
	}
}

static void
flip_horizontal(struct grid * const g)
{
	if (g->size == 2) {
		swap_columns(g, 0, 1);
		return;
	}

	if (g->size == 3) {
		swap_columns(g, 0, 2);
		return;
	}

	if (g->size == 4) {
		swap_columns(g, 0, 3);
		swap_columns(g, 1, 2);
		return;
	}

	fprintf(stderr, "%s: unhandled size: %zu\n", __func__, g->size);
}

static void
swap_columns(struct grid * const g, size_t const a, size_t const b)
{
	for (size_t i = 0; i < g->size; i++) {
		bool const tmp = g->pixels[i][a];
		g->pixels[i][a] = g->pixels[i][b];
		g->pixels[i][b] = tmp;
	}
}

static void
rotate(struct grid * const g)
{
	struct grid * const g2 = copy_grid(g);
	if (!g2) {
		fprintf(stderr, "new_grid()\n");
		return;
	}

	if (g->size == 2) {
		move_pixel(g, g2, 0, 0, 0, 1);
		move_pixel(g, g2, 0, 1, 1, 1);
		move_pixel(g, g2, 1, 1, 1, 0);
		move_pixel(g, g2, 1, 0, 0, 0);
		destroy_grid(g2);
		return;
	}

	if (g->size == 3) {
		move_pixel(g, g2, 0, 0, 0, 2);
		move_pixel(g, g2, 0, 1, 1, 2);
		move_pixel(g, g2, 0, 2, 2, 2);

		move_pixel(g, g2, 1, 2, 2, 1);
		move_pixel(g, g2, 2, 2, 2, 0);

		move_pixel(g, g2, 2, 1, 1, 0);
		move_pixel(g, g2, 2, 0, 0, 0);

		move_pixel(g, g2, 1, 0, 0, 1);

		destroy_grid(g2);
		return;
	}

	if (g->size == 4) {
		move_pixel(g, g2, 0, 0, 0, 3);
		move_pixel(g, g2, 0, 1, 1, 3);
		move_pixel(g, g2, 0, 2, 2, 3);
		move_pixel(g, g2, 0, 3, 3, 3);

		move_pixel(g, g2, 1, 3, 3, 2);
		move_pixel(g, g2, 2, 3, 3, 1);
		move_pixel(g, g2, 3, 3, 3, 0);

		move_pixel(g, g2, 3, 2, 2, 0);
		move_pixel(g, g2, 3, 1, 1, 0);
		move_pixel(g, g2, 3, 0, 0, 0);

		move_pixel(g, g2, 2, 0, 0, 1);
		move_pixel(g, g2, 1, 0, 0, 2);

		move_pixel(g, g2, 1, 1, 1, 2);
		move_pixel(g, g2, 1, 2, 2, 2);
		move_pixel(g, g2, 2, 2, 2, 1);
		move_pixel(g, g2, 2, 1, 1, 1);

		destroy_grid(g2);
		return;
	}

	fprintf(stderr, "%s: unhandled size: %zu\n", __func__, g->size);
	destroy_grid(g2);
}

static void
move_pixel(struct grid * const target,
		struct grid const * const source,
		size_t const source_row,
		size_t const source_col,
		size_t const target_row,
		size_t const target_col)
{
	target->pixels[target_row][target_col] =
			source->pixels[source_row][source_col];
}

static bool
rule_matches_grid(struct rule const * const r, struct grid const * const g)
{
	for (size_t i = 0; i < r->num_inputs; i++) {
		if (grids_match(r->inputs[i], g)) {
			return true;
		}
	}
	return false;
}

static bool
grids_match(struct grid const * const g0, struct grid const * const g1)
{
	if (g0->size != g1->size) {
		return false;
	}

	for (size_t row = 0; row < g0->size; row++) {
		for (size_t col = 0; col < g0->size; col++) {
			if (g0->pixels[row][col] != g1->pixels[row][col]) {
				return false;
			}
		}
	}

	return true;
}

static void
add_input_to_rule(struct rule * const r, struct grid const * const g)
{
	struct grid * const g2 = copy_grid(g);
	if (!g2) {
		fprintf(stderr, "copy_grid()\n");
		return;
	}

	if (r->num_inputs == MAX_INPUT_PATTERNS) {
		fprintf(stderr, "too many input patterns\n");
		return;
	}

	r->inputs[r->num_inputs] = g2;
	r->num_inputs++;
}

static void
print_rule(struct rule const * const r)
{
	printf("rule:\n");

	printf("  inputs (%zu):\n", r->num_inputs);
	for (size_t i = 0; i < r->num_inputs; i++) {
		printf("    input %zu:\n", i);
		print_grid(r->inputs[i]);
	}

	printf("  output:\n");
	print_grid(r->output);
}

static void
print_grid(struct grid const * const g)
{
	printf("      size: %zu capacity: %zu\n", g->size, g->capacity);

	for (size_t i = 0; i < g->size; i++) {
		printf("      ");
		for (size_t j = 0; j < g->size; j++) {
			if (g->pixels[i][j]) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}

static bool
run(struct rule ** const rules,
		size_t const num_rules,
		struct grid * const initial_grid,
		size_t const steps)
{
	struct grid * cur_grid = copy_grid(initial_grid);
	if (!cur_grid) {
		fprintf(stderr, "copy_grid()\n");
		return 1;
	}

	for (size_t i = 0; i < steps; i++) {
		if (DEBUG) {
			printf("step %zu: grid is currently size %zu:\n", i, cur_grid->size);
			print_grid(cur_grid);
		}

		if (cur_grid->size % 2 == 0) {
			struct grid * const g = apply_rules(rules, num_rules, cur_grid, 2);
			if (!g) {
				fprintf(stderr, "apply_rules()\n");
				destroy_grid(cur_grid);
				return false;
			}
			destroy_grid(cur_grid);
			cur_grid = g;
			continue;
		}

		if (cur_grid->size % 3 == 0) {
			struct grid * const g = apply_rules(rules, num_rules, cur_grid, 3);
			if (!g) {
				fprintf(stderr, "apply_rules()\n");
				destroy_grid(cur_grid);
				return false;
			}
			destroy_grid(cur_grid);
			cur_grid = g;
			continue;
		}

		fprintf(stderr, "unexpected size: %zu\n", cur_grid->size);
		destroy_grid(cur_grid);
		return false;
	}

	int pixels = 0;
	for (size_t row = 0; row < cur_grid->size; row++) {
		for (size_t col = 0; col < cur_grid->size; col++) {
			if (cur_grid->pixels[row][col]) {
				pixels++;
			}
		}
	}

	destroy_grid(cur_grid);
	printf("%d\n", pixels);
	return true;
}

static struct grid *
apply_rules(struct rule ** const rules,
		size_t const num_rules,
		struct grid const * const g,
		size_t const sz)
{
	struct grid * const g2 = new_grid(MAX_SIZE);
	if (!g2) {
		fprintf(stderr, "new_grid()\n");
		return NULL;
	}

	for (size_t row = 0; row < g->size / sz; row++) {
		for (size_t col = 0; col < g->size / sz; col++) {
			struct grid const * const output =
					match(rules, num_rules, g, row * sz, col * sz, sz);
			if (!output) {
				continue;
			}

			transpose_grid(g2, output, row * (sz + 1), col * (sz + 1));
		}
	}

	g2->size = g->size / sz * (sz + 1);

	return g2;
}

static struct grid *
match(struct rule ** const rules,
		size_t const num_rules,
		struct grid const * const g,
		size_t const target_row,
		size_t const target_col,
		size_t const sz)
{
	struct grid * const g2 = new_grid(sz);
	if (!g2) {
		fprintf(stderr, "new_grid()\n");
		return NULL;
	}
	g2->size = sz;

	for (size_t row = 0; row < sz; row++) {
		for (size_t col = 0; col < sz; col++) {
			g2->pixels[row][col] = g->pixels[target_row + row][target_col + col];
		}
	}

	for (size_t i = 0; i < num_rules; i++) {
		if (rule_matches_grid(rules[i], g2)) {
			destroy_grid(g2);
			return rules[i]->output;
		}
	}

	destroy_grid(g2);
	return NULL;
}
