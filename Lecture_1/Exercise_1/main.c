#include <stdio.h>
#include <stdlib.h>

#include "common.h"

int main()
{
	uint16_t _swap16 = 0x0201;
	printf("swap16() test: 0x%04X\n", swap16(_swap16));

	uint32_t _swap32 = 0x04030201;
	printf("swap32() test: 0x%08X\n", swap32(_swap32));

	uint64_t _swap64 = 0x0807060504030201;
	printf("swap64() test: 0x%I64X\n", swap64(_swap64));

	return 0;
}
