/// @file
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

/*!
 * Convert color from 3-byte [R][G][B] to 2-byte [R:5 G:6 B:5] representation.
 *
 * @param[in]	*rgb888	3-byte array address of color,
 *		as [R][G][B] (B - 0-index).
 * @return	2-bytes of RGB565 representation.
 *
 * @warning 	Low bit of every color will be dropped.
 */
uint16_t rgb888to565(uint8_t *rgb888)
{
	return (((rgb888[0] >> 3) << 0) |\
		((rgb888[1] >> 2) << 5) |\
		((rgb888[2] >> 3) << (5 + 6)));
}

/*!
 * Print to stdout binary representation of RGB565.
 *
 * Colors separated by space.
 * Example of output: 11111 111111 11111.
 *
 * @param[in]	input	2 byte RGB565 color representation for printing.
 *
 * @note	No new line at the end of output.
 */
void print_rgb565_bin(uint16_t input)
{
	for (int8_t i = 15; i >= 0; i--) {
		if (i == 10) {
			putchar(' ');
		}

		if (i == 4) {
			putchar(' ');
		}

		if (input & (1 << i)) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
}

int main()
{
	uint8_t _rgb888[3] = {0x1B, 0x1F, 0x1A};
	uint16_t _rgb565 = rgb888to565(_rgb888);

	printf("RGB888 hex: 0x%02X%02X%02X\n\n",
		_rgb888[0], _rgb888[1], _rgb888[2]);

	printf("RGB565 hex: 0x%04X\n", _rgb565);

	printf("RGB565 bin: ");
	print_rgb565_bin(_rgb565);

	return 0;
}

