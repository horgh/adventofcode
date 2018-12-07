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

static char
get_candidate(
		struct Instruction const * const,
		size_t const,
		char const * const,
		size_t const,
		int * const,
		int const * const
);
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
			bool found = false;
			for (size_t k = 0; k < n_starters; k++) {
				if (starters[k] == instructions[i].prereqs[j]) {
					found = true;
				}
			}
			if (!found) {
				starters[n_starters++] = instructions[i].prereqs[j];
			}
		}
	}
	//qsort(starters, n_starters, sizeof(char), cmp_char);
	int done[1024] = {0};
	//done[(int) starters[0]] = 1;
	//printf("%c", starters[0]);

//#define N_WORKERS 2
//#define STEP_BASE 0
#define N_WORKERS 5
#define STEP_BASE 60
	int seconds = 0;
	char worker_to_step[N_WORKERS] = {0};
	int step_to_seconds[64] = {0};
	size_t const total_needed = n_starters+n;
	printf("need %zu (%zu+%zu)\n", total_needed, n_starters, n);
	size_t total_done = 0;
	while (1) {
		for (int w = 0; w < N_WORKERS; w++) {
			if (worker_to_step[w] == 0) {
				continue;
			}

			char const c = worker_to_step[w];
			step_to_seconds[(int) c]--;
			if (step_to_seconds[(int) c] == 0) {
				done[(int) c] = 1;
				worker_to_step[w] = 0;
				total_done++;
				printf("%d: worker %d finished with %c\n", seconds, w, c);
			}
		}

		if (total_done == total_needed) {
			break;
		}

		for (int w = 0; w < N_WORKERS; w++) {
			if (worker_to_step[w] != 0) {
				continue;
			}

			char const c = get_candidate(
					instructions,
					n,
					starters,
					n_starters,
					done,
					step_to_seconds
					);
			if (c == -1) {
				continue;
			}
			printf("%d: worker %d starting on %c\n", seconds, w, c);
			worker_to_step[w] = c;
			step_to_seconds[(int) c] = c-64 + STEP_BASE;
		}

		seconds++;
	}

	printf("%d\n", seconds);
	return 0;
}

static char
get_candidate(
		struct Instruction const * const instructions,
		size_t const n,
		char const * const starters,
		size_t const n_starters,
		int * const done,
		int const * const in_progress
) {
	char candidates[1024] = {0};
	size_t m = 0;
	for (size_t i = 0; i < n; i++) {
		if (done[(int) instructions[i].step]) {
			continue;
		}
		if (in_progress[(int) instructions[i].step]) {
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
		}
	}

	for (size_t i = 0; i < n_starters; i++) {
		if (done[(int) starters[i]]) {
			continue;
		}
		if (in_progress[(int) starters[i]]) {
			continue;
		}
		candidates[m++] = starters[i];
	}

	qsort(candidates, m, sizeof(char), cmp_char);
	if (m == 0) {
		return -1;
	}
	return candidates[0];
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
