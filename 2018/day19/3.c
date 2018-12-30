#include <inttypes.h>
#include <stdio.h>

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	int r0 = 0;
	// int r2 = 0;
	int const r3 = 10551320;
	// int r4 = 10550400;
	int r5 = 1;

	uint64_t i = 0;
	while (1) {
		// printf("%" PRIu64 ": r5: %d (want %d)\n", i, r5, r3);
		// r2 = 1;

		// while (1) {
		//	r4 = r5*r2;

		//	if (r4 == r3) {
		//		r0 = r5+r0;
		//	}

		//	r2 = r2+1;

		//	if (r2 > r3) {
		//		break;
		//	}
		//}

		// Realized the above is just this:
		if (r3 % r5 == 0) {
			r0 += r5;
		}

		r5 = r5 + 1;

		if (r5 > r3) {
			printf("%d\n", r0);
			return 0;
		}

		i++;
	}

	return 0;
}
