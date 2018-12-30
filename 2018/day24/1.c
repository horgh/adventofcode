#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#define N_WEAKNESSES 16
#define ATTACK_SZ 32

enum Type { Immune, Infection };

struct Group;

struct Group {
	int id;
	enum Type type;
	int units;
	int hp;
	char immunities[N_WEAKNESSES][ATTACK_SZ];
	size_t n_immunities;
	char weaknesses[N_WEAKNESSES][ATTACK_SZ];
	size_t n_weaknesses;
	int ap;
	char attack[ATTACK_SZ];
	int initiative;
	struct Group * attacking;
};

static char const * parse_immunities_or_weaknesses(
		char const * const, struct Group * const, bool const);

static void print_group(struct Group const * const);

static int count_units(struct Group const * const, size_t const);

static int cmp_groups_selection(void const * const, void const * const);

static void choose_target(
		struct Group ** const, size_t const, struct Group * const);

static uint64_t calc_damage(
		struct Group const * const, struct Group const * const);

static int cmp_groups_attacking(void const * const, void const * const);

static void attack(struct Group * const);

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Group immunes[128] = {0};
	size_t n_immunes = 0;
	struct Group infections[128] = {0};
	size_t n_infections = 0;
	bool in_infections = false;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);
		if (strlen(buf) == 0) {
			continue;
		}

		char const * ptr = buf;
		if (strcmp(ptr, "Immune System:") == 0) {
			continue;
		}
		if (strcmp(ptr, "Infection:") == 0) {
			in_infections = true;
			continue;
		}

		struct Group * g = immunes;
		size_t * n = &n_immunes;
		if (in_infections) {
			g = infections;
			n = &n_infections;
		}

		g[*n].id = (int)*n + 1;
		if (!in_infections) {
			g[*n].type = Immune;
		} else {
			g[*n].type = Infection;
		}

		g[*n].units = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += strlen(" units each with ");
		g[*n].hp = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += strlen(" hit points ");
		if (*ptr == '(') {
			ptr++;
			if (strncmp(ptr, "immune to ", strlen("immune to ")) == 0) {
				ptr += strlen("immune to ");
				ptr = parse_immunities_or_weaknesses(ptr, &g[*n], true);
				if (*ptr == ';') {
					assert(strncmp(ptr, "; weak to ", strlen("; weak to")) == 0);
					ptr += strlen("; weak to ");
					ptr = parse_immunities_or_weaknesses(ptr, &g[*n], false);
				}
			}
			if (strncmp(ptr, "weak to ", strlen("weak to ")) == 0) {
				ptr += strlen("weak to ");
				ptr = parse_immunities_or_weaknesses(ptr, &g[*n], false);
				if (*ptr == ';') {
					assert(strncmp(ptr, "; immune to ", strlen("; immune to")) == 0);
					ptr += strlen("; immune to ");
					ptr = parse_immunities_or_weaknesses(ptr, &g[*n], true);
				}
			}
			assert(strncmp(ptr, ") ", strlen(") ")) == 0);
			ptr += 2;
		}
		if (strncmp(ptr,
						"with an attack that does ",
						strlen("with an attack that does ")) != 0) {
			printf("parsing expecting attack power, but at [%s]\n", ptr);
			assert(1 == 0);
		}
		ptr += strlen("with an attack that does ");
		g[*n].ap = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;
		size_t i = 0;
		while (isalpha(*ptr)) {
			g[*n].attack[i++] = *ptr;
			assert(i + 1 != ATTACK_SZ);
			ptr++;
		}
		assert(strncmp(ptr,
							 " damage at initiative ",
							 strlen(" damage at initiative ")) == 0);
		ptr += strlen(" damage at initiative ");
		g[*n].initiative = atoi(ptr);
		*n += 1;
	}

	if (false) {
		printf("Immune System:\n");
		for (size_t i = 0; i < n_immunes; i++) {
			print_group(&immunes[i]);
		}
		printf("\n");
		printf("Infection:\n");
		for (size_t i = 0; i < n_infections; i++) {
			print_group(&infections[i]);
		}
	}

	size_t const n_all_groups = n_immunes + n_infections;
	struct Group ** const all_groups =
			calloc(n_all_groups, sizeof(struct Group *));
	assert(all_groups != NULL);
	{
		size_t i = 0;
		for (size_t j = 0; j < n_immunes; j++) {
			all_groups[i++] = &immunes[j];
		}
		for (size_t j = 0; j < n_infections; j++) {
			all_groups[i++] = &infections[j];
		}
	}

	int round = 0;
	while (1) {
		if (count_units(immunes, n_immunes) == 0 ||
				count_units(infections, n_infections) == 0) {
			break;
		}

		if (false) {
			printf("round %d\n", round);
			printf("Immune System:\n");
			for (size_t i = 0; i < n_immunes; i++) {
				printf("Group %d contains %d units\n", immunes[i].id, immunes[i].units);
			}
			printf("Infection:\n");
			for (size_t i = 0; i < n_infections; i++) {
				printf("Group %d contains %d units\n",
						infections[i].id,
						infections[i].units);
			}
			printf("\n");
			for (size_t i = 0; i < n_infections; i++) {
				if (infections[i].units == 0) {
					continue;
				}
				for (size_t j = 0; j < n_immunes; j++) {
					if (immunes[j].units == 0) {
						continue;
					}
					uint64_t damage = calc_damage(&infections[i], &immunes[j]);
					printf("Infection group %d would deal defending group %d %" PRIu64
								 " damage\n",
							infections[i].id,
							immunes[j].id,
							damage);
				}
			}
			for (size_t i = 0; i < n_immunes; i++) {
				if (immunes[i].units == 0) {
					continue;
				}
				for (size_t j = 0; j < n_infections; j++) {
					if (infections[j].units == 0) {
						continue;
					}
					uint64_t damage = calc_damage(&immunes[i], &infections[j]);
					printf("Immune System group %d would deal defending group %d %" PRIu64
								 " damage\n",
							immunes[i].id,
							infections[j].id,
							damage);
				}
			}
			printf("\n");
		}

		qsort(
				all_groups, n_all_groups, sizeof(struct Group *), cmp_groups_selection);
		for (size_t i = 0; i < n_all_groups; i++) {
			all_groups[i]->attacking = NULL;
		}
		for (size_t i = 0; i < n_all_groups; i++) {
			choose_target(all_groups, n_all_groups, all_groups[i]);
		}

		qsort(
				all_groups, n_all_groups, sizeof(struct Group *), cmp_groups_attacking);
		for (size_t i = 0; i < n_all_groups; i++) {
			attack(all_groups[i]);
		}
		round++;
	}

	int const units =
			count_units(immunes, n_immunes) + count_units(infections, n_infections);
	printf("%d\n", units);
	return 0;
}

static char const *
parse_immunities_or_weaknesses(
		char const * const buf, struct Group * const g, bool const immunity)
{
	char const * ptr = buf;
	while (1) {
		size_t i = 0;
		while (isalpha(*ptr)) {
			if (immunity) {
				g->immunities[g->n_immunities][i++] = *ptr;
			} else {
				g->weaknesses[g->n_weaknesses][i++] = *ptr;
			}
			assert(i + 1 != ATTACK_SZ);
			ptr++;
		}
		if (immunity) {
			g->n_immunities++;
		} else {
			g->n_weaknesses++;
		}
		if (*ptr == ',') {
			ptr += 2;
			continue;
		}
		if (*ptr == ';' || *ptr == ')') {
			return ptr;
		}
		printf("parsing immunities/weaknesses, at [%s]\n", ptr);
		assert(1 == 0);
	}
}

static void
print_group(struct Group const * const g)
{
	printf("%d units each with %d hit points ", g->units, g->hp);
	if (g->n_immunities > 0 || g->n_weaknesses > 0) {
		printf("(");
	}
	if (g->n_immunities > 0) {
		printf("immune to ");
		for (size_t i = 0; i < g->n_immunities; i++) {
			printf("%s", g->immunities[i]);
			if (i + 1 < g->n_immunities) {
				printf(", ");
			}
		}
		if (g->n_weaknesses == 0) {
			printf(") ");
		} else {
			printf("; ");
		}
	}
	if (g->n_weaknesses > 0) {
		printf("weak to ");
		for (size_t i = 0; i < g->n_weaknesses; i++) {
			printf("%s", g->weaknesses[i]);
			if (i + 1 < g->n_weaknesses) {
				printf(", ");
			}
		}
		printf(") ");
	}
	printf("with an attack that does %d %s damage at initiative %d\n",
			g->ap,
			g->attack,
			g->initiative);
}

static int
count_units(struct Group const * const g, size_t const n)
{
	int count = 0;
	for (size_t i = 0; i < n; i++) {
		count += g[i].units;
	}
	return count;
}

__attribute__((pure)) static int
cmp_groups_selection(void const * const a_v, void const * const b_v)
{
	struct Group * const * const aa = a_v;
	struct Group * const * const bb = b_v;
	struct Group const * const a = *aa;
	struct Group const * const b = *bb;

	if (a->units * a->ap < b->units * b->ap) {
		return 1;
	}
	if (a->units * a->ap > b->units * b->ap) {
		return -1;
	}
	if (a->initiative < b->initiative) {
		return 1;
	}
	if (a->initiative > b->initiative) {
		return -1;
	}
	assert(1 == 0);
	return 0;
}

static void
choose_target(struct Group ** const all_groups,
		size_t const n_all_groups,
		struct Group * const g)
{
	if (g->units == 0) {
		return;
	}
	struct Group * target = NULL;
	uint64_t target_damage = 0;
	for (size_t i = 0; i < n_all_groups; i++) {
		struct Group * const g2 = all_groups[i];
		if (g2->type == g->type) {
			continue;
		}
		if (g2->units == 0) {
			continue;
		}
		bool is_being_attacked = false;
		for (size_t j = 0; j < n_all_groups; j++) {
			if (all_groups[j]->attacking == g2) {
				is_being_attacked = true;
				break;
			}
		}
		if (is_being_attacked) {
			continue;
		}
		uint64_t const damage = calc_damage(g, g2);
		if (damage == 0) {
			continue;
		}
		if (!target) {
			target = g2;
			target_damage = damage;
			continue;
		}
		if (damage < target_damage) {
			continue;
		}
		if (damage > target_damage) {
			target = g2;
			target_damage = damage;
			continue;
		}
		if (g2->units * g2->ap < target->units * target->ap) {
			continue;
		}
		if (g2->units * g2->ap > target->units * target->ap) {
			target = g2;
			target_damage = damage;
			continue;
		}
		if (g2->initiative < target->initiative) {
			continue;
		}
		if (g2->initiative > target->initiative) {
			target = g2;
			target_damage = damage;
			continue;
		}
		assert(1 == 0);
	}
	g->attacking = target;
}

__attribute__((pure)) static uint64_t
calc_damage(
		struct Group const * const attacker, struct Group const * const defender)
{
	for (size_t i = 0; i < defender->n_immunities; i++) {
		if (strcmp(defender->immunities[i], attacker->attack) == 0) {
			return 0;
		}
	}
	uint64_t const damage = (uint64_t)(attacker->ap * attacker->units);
	for (size_t i = 0; i < defender->n_weaknesses; i++) {
		if (strcmp(defender->weaknesses[i], attacker->attack) == 0) {
			return damage * 2;
		}
	}
	return damage;
}

__attribute__((pure)) static int
cmp_groups_attacking(void const * const a_v, void const * const b_v)
{
	struct Group * const * const aa = a_v;
	struct Group * const * const bb = b_v;
	struct Group const * const a = *aa;
	struct Group const * const b = *bb;

	if (a->initiative < b->initiative) {
		return 1;
	}
	if (a->initiative > b->initiative) {
		return -1;
	}
	assert(1 == 0);
	return 0;
}

static void
attack(struct Group * const attacker)
{
	struct Group * const defender = attacker->attacking;
	if (!defender) {
		return;
	}
	uint64_t const damage = calc_damage(attacker, defender);

	uint64_t const units_lost = damage / (uint64_t)defender->hp;
	defender->units -= (int)units_lost;
	if (defender->units < 0) {
		defender->units = 0;
	}
	if (false) {
		if (attacker->type == Immune) {
			printf("Immune System ");
		} else {
			printf("Infection ");
		}
		printf("group %d attacks defending group %d, killing %" PRIu64
					 " units (damage %" PRIu64 ")\n",
				attacker->id,
				defender->id,
				units_lost,
				damage);
	}
	if (units_lost == 0) {
		if (false) {
			printf("no units lost\n");
		}
	}
}
