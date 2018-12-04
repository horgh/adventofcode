#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

enum Action {
	Begin,
	Sleep,
	Wake
};

struct Record {
	int month;
	int day;
	int hour;
	int minute;
	enum Action action;
	int id;
};

static int
cmp_record(void const * const, void const * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct Record records[2048] = {0};
	size_t n = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		char const * ptr = buf;

		ptr++;
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;

		records[n].month = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;

		records[n].day = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;

		records[n].hour = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr++;

		records[n].minute = atoi(ptr);
		while (isdigit(*ptr)) {
			ptr++;
		}
		ptr += 2;

		if (strncmp(ptr, "Guard #", strlen("Guard #")) == 0) {
			ptr += strlen("Guard #");
			records[n].id = atoi(ptr);
			records[n].action = Begin;
			n++;
			continue;
		}
		if (strcmp(ptr, "falls asleep") == 0) {
			records[n].action = Sleep;
			n++;
			continue;
		}
		if (strcmp(ptr, "wakes up") == 0) {
			records[n].action = Wake;
			n++;
			continue;
		}
		assert(1 == 0);
	}

	qsort(records, n, sizeof(struct Record), cmp_record);

#ifdef DEBUG
	for (size_t i = 0; i < n; i++) {
		struct Record r = records[i];
		printf("%02d-%02d %02d:%02d #%d ",
				r.month,
				r.day,
				r.hour,
				r.minute,
				r.id
				);
		switch (r.action) {
		case Begin:
			printf("begin");
			break;
		case Sleep:
			printf("sleep");
			break;
		case Wake:
			printf("wake");
			break;
		default:
			assert(1 == 0);
		}
		printf("\n");
	}
#endif
	int current_id = -1;
	int start_minute = -1;
	struct htable * const h = hash_init(1024);
	assert(h != NULL);
	int guard_to_minutes[10240][60] = {0};
	for (size_t i = 0; i < n; i++) {
		struct Record r = records[i];

		if (r.action == Begin) {
			current_id = r.id;
			if (hash_has_key_i(h, current_id)) {
				continue;
			}
			int * const minutes = calloc(1, sizeof(int));
			assert(minutes != NULL);
			assert(hash_set_i(h, current_id, minutes));
			continue;
		}

		if (r.action == Sleep) {
			start_minute = r.minute;
			continue;
		}

		if (r.action == Wake) {
			int * const minutes = hash_get_i(h, current_id);
			assert(minutes != NULL);
			*minutes += r.minute - start_minute;
			assert(hash_set_i(h, current_id, minutes));
			for (int j = start_minute; j < r.minute; j++) {
				guard_to_minutes[current_id][j]++;
				//printf("%d slept minute %d\n", current_id, j);
			}
			continue;
		}

		assert(1 == 0);
	}

	void * * const ids = hash_get_keys(h);
	int most_asleep_id = -1;
	int most_asleep_minutes = -1;
	for (size_t i = 0; ids[i]; i++) {
		int const * const id = ids[i];
		int const * const minutes = hash_get_i(h, *id);
		assert(minutes != NULL);
		//printf("%d slept for %d\n", *id, *minutes);
		if (*minutes > most_asleep_minutes) {
			most_asleep_id = *id;
			most_asleep_minutes = *minutes;
		}
	}

	int sleep_times = -1;
	int most_asleep_minute = -1;
	for (int i = 0; i < 60; i++) {
		if (guard_to_minutes[most_asleep_id][i] <= sleep_times) {
			continue;
		}
		sleep_times = guard_to_minutes[most_asleep_id][i];
		most_asleep_minute = i;
	}

	//printf("%d\n", most_asleep_id);
	//printf("%d\n", most_asleep_minute);
	printf("%d\n", most_asleep_id*most_asleep_minute);
	return 0;
}

static int
cmp_record(void const * const a, void const * const b)
{
	struct Record const * const ar = a;
	struct Record const * const br = b;
	if (ar->month != br->month) {
		if (ar->month < br->month) {
			return -1;
		}
		return 1;
	}
	if (ar->day != br->day) {
		if (ar->day < br->day) {
			return -1;
		}
		return 1;
	}
	if (ar->hour != br->hour) {
		if (ar->hour < br->hour) {
			return -1;
		}
		return 1;
	}
	if (ar->minute != br->minute) {
		if (ar->minute < br->minute) {
			return -1;
		}
		return 1;
	}
	return 0;
}
