/** @file
 * GL EmbeddedProCamp - Task#1
 *
 * Create void function that copies one string to another.
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>

/** \brief 	Copy \b 0-terminated string from \p source_ptr to \p target_ptr.
 *
 * \param[out]	*target_ptr	Pointer to target/out string.
 * \param[in]	*source_ptr	Pointer to source/in string.
 *
 * \pre		Source string must be 0-terminated.
 */
void strint_cpy(uint8_t *target_ptr, uint8_t *source_ptr)
{
	while (*source_ptr != 0) {
		*target_ptr = *source_ptr;

		target_ptr++;
		source_ptr++;
	}
	*target_ptr = '\0';
}

int main()
{
	uint8_t msg[] = "Hello world!!!";

	printf("Source string: %s\n", msg);
	printf("length: %d\n", strlen(msg));

	uint8_t buff[50];
	strint_cpy(buff, msg);

	printf("\nTarget string: %s\n", buff);
	printf("length: %d\n", strlen(msg));

	return 0;
}
