#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <inttypes.h>
#include <map.h>
#include <stdio.h>
#include <string.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int32_t const r0 = 7224965;
	int32_t r5 = 0;
	struct htable * const h = hash_init(1024);
	assert(h != NULL);
	while (1) {
		int32_t r4 = r5|65536;
		r5 = 13284195;

		while (1) {
			int32_t r3 = r4&255;
			r5 += r3;
			r5 &= 16777215;
			r5 *= 65899;
			r5 &= 16777215;
			if (256 > r4) {
				break;
			}

			r3 = 0;
			while (1) {
				int32_t r2 = r3+1;
				r2 *= 256;
				if (r2 > r4) {
					break;
				}
				r3++;
			}

			r4 = r3;
		}

		char buf[20] = {0};
		sprintf(buf, "%" PRId32, r5);
		if (!hash_has_key(h, buf)) {
			assert(hash_set(h, buf, NULL));
			printf("%" PRId32 "\n", r5);
		}

		if (r5 == r0) {
			break;
		}
	}

	return 0;
}
