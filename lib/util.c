#include <ctype.h>
#include "util.h"

void
trim_right(char * const s)
{
	char * ptr = s;
	while (*ptr != '\0') {
		ptr++;
	}
	if (ptr == s) {
		return;
	}
	ptr--;
	while (ptr != s) {
		if (!isspace(*ptr)) {
			break;
		}
		*ptr = '\0';
		ptr--;
	}
}

#ifdef TEST_UTIL

#include <assert.h>
#include <string.h>

int main(void)
{
	char s[128] = {0};

	trim_right(s);
	assert(strcmp(s, "") == 0);

	strcat(s, "hi");
	trim_right(s);
	assert(strcmp(s, "hi") == 0);

	memset(s, 0, sizeof(s));
	strcat(s, "hi ");
	trim_right(s);
	assert(strcmp(s, "hi") == 0);

	memset(s, 0, sizeof(s));
	strcat(s, "hi  \n");
	trim_right(s);
	assert(strcmp(s, "hi") == 0);

	return 0;
}

#endif
