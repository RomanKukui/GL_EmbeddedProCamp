#include "common.h"

uint16_t swap16(uint16_t input)
{
	return (input >> 8) | (input << 8);
}

uint32_t swap32(uint32_t input)
{
	input = ((input << 8) & 0xFF00FF00) |\
		((input >> 8) & 0x00FF00FF);
	return (input >> 16) | (input << 16);
}

uint64_t swap64(uint64_t input)
{
        input = ((input << 8) & 0xFF00FF00FF00FF00) |\
		((input >> 8) & 0x00FF00FF00FF00FF);
	input = ((input << 16) & 0xFFFF0000FFFF0000) |\
		((input >> 16) & 0x0000FFFF0000FFFF);
	input = ((input << 32) & 0xFFFFFFFF00000000) |\
		((input >> 32) & 0x00000000FFFFFFFF);
	return input;
}
