/*!
 * @file
 *
 * GL EmbeddedProCamp - Task#1
 *
 * Task: Create function to copy all \b digits
 * from input string to output string
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>

//! Test, if char in range of ANSI-table digits(0x30~0x39 char code).
#define IS_ANSI_DIGIT(char) \
	(((uint8_t)char >= 0x30) && ((uint8_t)char <= 0x39))

/*!
 * Calculate number of digits (ANSI code 0x30~0x39) in 0-terminated string.
 *
 * @param[in]	in_str_ptr Source string(0-terminated) pointer.
 *
 * @return	Number of digits chars in in_str_ptr string.
 */
uint16_t digit_num(uint8_t *in_str_ptr)
{
 	uint16_t digits_num = 0;

 	for (int16_t i = strlen(in_str_ptr) - 1; i >= 0; i--) {
		if (IS_ANSI_DIGIT(in_str_ptr[i])) {
			digits_num++;
		}
	}

	return digits_num;
}

/*!
 * Copy chars that contain ANSI-digits(code 0x30-0x39)
 * from in_str to out_str.
 *
 * @param[in]	in_str_ptr Source string pointer.
 * @param[out]	out_str_ptr Target string pointer, where digits will be putted.
 *
 * @note	in_str_ptr String must be 0-terminated.
 */
void digit_cpy(uint8_t *in_str_ptr, uint8_t *out_str_ptr)
{
	size_t chars_num = strlen(in_str_ptr);

	uint16_t j = 0;
	for (uint16_t i = 0; i < chars_num; i++) {
		if (IS_ANSI_DIGIT(in_str_ptr[i])) {
			out_str_ptr[j] = in_str_ptr[i];
			j++;
		}
	}
	out_str_ptr[j] = '\0';
}

int main()
{
	printf("Please enter some string with digits\n");
	printf("! less than 50 chars, without spaces !\n\n");
	uint8_t in_buff[50];
	scanf("%50s", in_buff);

	// target string memory allocating (unnecessary memory not allocated)
	uint8_t *digit_str_ptr = NULL;
	digit_str_ptr = malloc(sizeof(uint8_t) * digit_num(in_buff) + 1);
	if (digit_str_ptr == NULL) {
		printf("Error: memory allocation error.");
		return -1;
	}

	digit_cpy(in_buff, digit_str_ptr);

	printf("Target string, non-digits filtered:\n%s",\
		digit_str_ptr);

	free(digit_str_ptr);

	return 0;
}
