#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
__ipv7_supports_tls(const char * const);
static bool
__is_abba(const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	assert(__is_abba("abba"));
	assert(!__is_abba("abcd"));

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	int count = 0;
	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		if (__ipv7_supports_tls(buf)) {
			count++;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%d\n", count);
	return 0;
}

static bool
__ipv7_supports_tls(const char * const ip)
{
	if (!ip || strlen(ip) == 0) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));
	size_t buf_i = 0;

	bool has_abba = false;

	const char * ptr = ip;
	while (*ptr) {
		if (*ptr == '[') {
			if (strlen(buf) > 0) {
				if (!has_abba) {
					has_abba = __is_abba(buf);
				}
			}

			memset(buf, 0, sizeof(buf));
			buf_i = 0;
			ptr++;
			continue;
		}

		if (*ptr == ']') {
			if (strlen(buf) > 0) {
				if (__is_abba(buf)) {
					return false;
				}
			}

			memset(buf, 0, sizeof(buf));
			buf_i = 0;
			ptr++;
			continue;
		}

		if (buf_i >= sizeof(buf)-1) {
			printf("ipv7 too long\n");
			return false;
		}

		buf[buf_i] = *ptr;
		buf_i++;
		ptr++;
	}

	if (strlen(buf) > 0) {
		if (!has_abba) {
			has_abba = __is_abba(buf);
		}
	}

	return has_abba;
}

static bool
__is_abba(const char * const s)
{
	if (!s || strlen(s) == 0) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	for (size_t i = 0; i < strlen(s); i++) {
		if (i+3 < strlen(s)) {
			if (s[i] != s[i+1] &&
					s[i+1] == s[i+2] &&
					s[i] == s[i+3]) {
#ifdef DEBUG
				printf("%s is abba\n", s);
#endif
				return true;
			}
		}
	}

#ifdef DEBUG
	printf("%s is not abba\n", s);
#endif
	return false;
}
