#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

static int fuel_mass(int const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int total_mass = 0;
	while (1) {
		char buf[256] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		int const mass = atoi(buf);
		total_mass += fuel_mass(mass);
	}

	printf("%d\n", total_mass);
	return 0;
}

__attribute__((const))
static int fuel_mass(int const mass) {
	int const mass2 = mass/3-2;
	if (mass2 <= 0) {
		return 0;
	}
	return mass2 + fuel_mass(mass2);
}
