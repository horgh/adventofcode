#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
	int64_t b = 105700;
	int64_t d = 0;
	int64_t e = 0;
	int64_t f = 0;
	int64_t g = 0;
	int64_t h = 0;

	while (1) {
		f = 1;
		d = 2;

		while (1) {
			e = 2;

			while (1) {
				g = d * e - b;
				if (g == 0) {
					f = 0;
				}
				e++;

				g = e - b;
				if (g == 0) {
					break;
				}
			}

			d++;

			g = d - b;
			if (g == 0) {
				break;
			}
		}

		if (f == 0) {
			h++;
		}

		g = b - 122700;
		printf("%" PRId64 "... (h=%" PRId64 ", f=%" PRId64 ")\n", g, h, f);
		if (g == 0) {
			break;
		}

		b += 17;
	}

	printf("%" PRId64 "\n", h);
	return 0;
}
