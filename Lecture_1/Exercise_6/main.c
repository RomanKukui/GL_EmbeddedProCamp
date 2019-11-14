#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

/*!
 * Print all aliquot numbers in range 0-500.
 *
 * @param[in]	in	Aliquot-to, value. If out of range - print nothing.
 */
void print_aliquot(uint16_t in)
{
	for (uint16_t i = in; i <= 500; i += in) {
		printf("%u\n", i);
	}
}

int main()
{
	printf("Please input a number: ");

	uint16_t user_in = 0;
	scanf("%u", &user_in);

	print_aliquot(user_in);

	return 0;
}
