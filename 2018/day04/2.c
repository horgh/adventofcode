#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
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

	int current_id = -1;
	int start_minute = -1;
	int guard_to_sleep_time[10240] = {0};
	int guard_to_minutes[10240][60] = {0};
	for (size_t i = 0; i < n; i++) {
		struct Record r = records[i];

		if (r.action == Begin) {
			current_id = r.id;
			continue;
		}

		if (r.action == Sleep) {
			start_minute = r.minute;
			continue;
		}

		if (r.action == Wake) {
			guard_to_sleep_time[current_id] += r.minute - start_minute;
			for (int j = start_minute; j < r.minute; j++) {
				guard_to_minutes[current_id][j]++;
			}
			continue;
		}

		assert(1 == 0);
	}

	int times_slept = -1;
	int interesting_id = -1;
	int minute_he_slept_most = -1;
	for (size_t i = 0; i < 10240; i++) {
		for (int j = 0; j < 60; j++) {
			if (guard_to_minutes[i][j] < times_slept) {
				continue;
			}
			times_slept = guard_to_minutes[i][j];
			interesting_id = (int) i;
			minute_he_slept_most = j;
		}
	}

	printf("%d\n", interesting_id*minute_he_slept_most);
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
