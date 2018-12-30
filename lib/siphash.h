#ifndef __SIPHASH_H
#define __SIPHASH_H

#include <stdint.h>

int siphash(
		const uint8_t *, const size_t, const uint8_t *, uint8_t *, const size_t);

#endif
