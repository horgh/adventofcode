#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Instruction {
	char prereqs[128];
	size_t n_prereqs;
	char step;
};

static int
cmp_char(void const * const, void const * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Instruction instructions[1024] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;

		ptr += strlen("Step ");
		char const prereq = *ptr;
		ptr += 1+strlen(" must be finished before step ");
		char const step = *ptr;

		bool found = false;
		for (size_t i = 0; i < n; i++) {
			if (instructions[i].step != step) {
				continue;
			}
			instructions[i].prereqs[instructions[i].n_prereqs++] = prereq;
			found = true;
		}
		if (!found) {
			instructions[n].prereqs[instructions[n].n_prereqs++] = prereq;
			instructions[n++].step = step;
		}
	}

	for (size_t i = 0; i < n; i++) {
		printf("%c has prereqs:\n", instructions[i].step);
		for (size_t j = 0; j < instructions[i].n_prereqs; j++) {
			printf("  %c\n", instructions[i].prereqs[j]);
		}
	}

	int seen[1024] = {0};
	for (size_t i = 0; i < n; i++) {
		seen[(int) instructions[i].step] = 1;
	}

	char starters[1024] = {0};
	size_t n_starters = 0;
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < instructions[i].n_prereqs; j++) {
			if (seen[(int) instructions[i].prereqs[j]]) {
				continue;
			}
			starters[n_starters++] = instructions[i].prereqs[j];
		}
	}
	qsort(starters, n_starters, sizeof(char), cmp_char);
	int done[1024] = {0};
	done[(int) starters[0]] = 1;
	printf("%c", starters[0]);

	while (1) {
		char candidates[1024] = {0};
		size_t m = 0;
		for (size_t i = 0; i < n; i++) {
			if (done[(int) instructions[i].step]) {
				continue;
			}
			bool all_prereqs_done = true;
			for (size_t j = 0; j < instructions[i].n_prereqs; j++) {
				if (done[(int) instructions[i].prereqs[j]]) {
					continue;
				}
				all_prereqs_done = false;
			}
			if (all_prereqs_done) {
				candidates[m++] = instructions[i].step;
				//printf("%c is avail\n", instr.step);
			}
		}

		for (size_t i = 0; i < n_starters; i++) {
			if (done[(int) starters[i]]) {
				continue;
			}
			candidates[m++] = starters[i];
		}

		if (m == 0) {
			break;
		}
		qsort(candidates, m, sizeof(char), cmp_char);
		//printf("%zu candidates\n", m);
		//for (size_t i = 0; i < m; i++) {
		//	printf(" ... %c\n", candidates[i]);
		//}
		//printf("choosing %c\n", candidates[0]);
	  printf("%c", candidates[0]);
		done[(int) candidates[0]] = 1;
		//count_done++;
	}

	printf("\n");
	return 0;
}

static int
cmp_char(void const * const a, void const * const b)
{
	char const * const a_c = a;
	char const * const b_c = b;
	if (*a_c < *b_c) {
		return -1;
	}
	if (*a_c > *b_c) {
		return 1;
	}
	return 0;
}
