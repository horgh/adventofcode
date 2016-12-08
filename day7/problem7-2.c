#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct List;

struct List {
	char * s;
	struct List * next;
};

static bool
__ipv7_supports_ssl(const char * const);
static struct List *
__get_abas(struct List *, const char * const);
static bool
__is_aba(const char * const);
static struct List *
__append_list(struct List *, const char * const);
static void
__destroy_list(struct List * const);
static void
__print_list(const struct List * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	assert(__is_aba("aba"));
	assert(!__is_aba("abc"));
	assert(!__is_aba("cdb"));
	assert(!__is_aba("b"));

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		if (__ipv7_supports_ssl(buf)) {
			printf("SSL: %s\n", buf);
		} else {
			printf("No SSL: %s\n", buf);
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

static bool
__ipv7_supports_ssl(const char * const ip)
{
	if (!ip || strlen(ip) == 0) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));
	size_t buf_i = 0;

	struct List * abas = NULL;
	struct List * babs = NULL;

	const char * ptr = ip;
	while (*ptr) {
		if (*ptr == '[') {
			abas = __get_abas(abas, buf);

			memset(buf, 0, sizeof(buf));
			buf_i = 0;
			ptr++;
			continue;
		}

		if (*ptr == ']') {
			babs = __get_abas(babs, buf);

			memset(buf, 0, sizeof(buf));
			buf_i = 0;
			ptr++;
			continue;
		}

		if (buf_i >= sizeof(buf)-1) {
			printf("ipv7 too long\n");
			__destroy_list(abas);
			__destroy_list(babs);
			return false;
		}

		buf[buf_i] = *ptr;
		buf_i++;
		ptr++;
	}

	abas = __get_abas(abas, buf);

	if (false) {
		printf("==========\n");
		printf("IP: %s\n", ip);
		printf("abas:\n");
		__print_list(abas);
		printf("\n");
		printf("babs:\n");
		__print_list(babs);
		printf("\n");
	}

	const struct List * abas_ptr = abas;
	while (abas_ptr) {
		const char * const aba = abas_ptr->s;

		const struct List * babs_ptr = babs;
		while (babs_ptr) {
			const char * const bab = babs_ptr->s;

			if (aba[0] == bab[1] && aba[1] == bab[0]) {
				__destroy_list(abas);
				__destroy_list(babs);
				return true;
			}

			babs_ptr = babs_ptr->next;
		}

		abas_ptr = abas_ptr->next;
	}

	__destroy_list(abas);
	__destroy_list(babs);
	return false;
}

static struct List *
__get_abas(struct List * l, const char * const s)
{
	// l may be empty.
	// s may be 0 length.
	if (!s) {
		printf("%s\n", strerror(EINVAL));
		return NULL;
	}

	for (size_t i = 0; i < strlen(s); i++) {
		if (__is_aba(s+i)) {
			l = __append_list(l, s+i);
		}
	}

	return l;
}

static bool
__is_aba(const char * const s)
{
	if (!s || strlen(s) == 0) {
		printf("%s\n", strerror(EINVAL));
		return false;
	}

	return strlen(s) >= 3 &&
		s[0] != s[1] &&
		s[0] == s[2];
}

static struct List *
__append_list(struct List * l, const char * const s)
{
	if (!s || strlen(s) < 3) {
		printf("%s\n", strerror(EINVAL));
		return NULL;
	}

	char * const aba = calloc(4, sizeof(char));
	if (!aba) {
		printf("%s\n", strerror(ENOMEM));
		return NULL;
	}
	aba[0] = s[0];
	aba[1] = s[1];
	aba[2] = s[2];

	if (!l) {
		l = calloc(1, sizeof(struct List));
		if (!l) {
			printf("%s\n", strerror(ENOMEM));
			free(aba);
			return NULL;
		}

		l->s = aba;

		return l;
	}

	struct List * ptr = l;
	while (ptr->next) {
		ptr = ptr->next;
	}

	ptr->next = calloc(1, sizeof(struct List));
	if (!ptr->next) {
		printf("%s\n", strerror(ENOMEM));
		free(aba);
		return NULL;
	}

	ptr->next->s = aba;

	return l;
}

static void
__destroy_list(struct List * const l)
{
	struct List * ptr = l;
	while (ptr) {
		struct List * next = ptr->next;
		free(ptr->s);
		free(ptr);
		ptr = next;
	}
}

static void
__print_list(const struct List * const l)
{
	const struct List * ptr = l;
	while (ptr) {
		const char * const s = ptr->s;
		printf("%c%c%c\n", s[0], s[1], s[2]);
		ptr = ptr->next;
	}
}
