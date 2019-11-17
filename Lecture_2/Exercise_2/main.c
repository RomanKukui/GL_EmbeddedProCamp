/** @file
 *
 * GL EmbeddedProCamp - Task#2
 *
 * Task: Create function to find the biggest element in integer array.
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

/** \brief	Search and return biggest integer in array.
 *
 * \param[in]	*array 	Pointer to first element of array,
 *			where will be max value searched.
 * \param[in]	length	Length of array[] in elements (!not in bytes).
 * \return	Maximum value, that stored in array.
 */
int32_t max_int32(int32_t *array, size_t length)
{
	int32_t max = array[0];

	for (size_t i = 1; i < length; i++) {
		if (array[i] > max) {
			max = array[i];
		}
	}

	return max;
}

int main()
{
	int32_t src_array[] = {4, 5, 8, 54, 108, 15, 2, -5, 25};
	size_t length = sizeof(src_array)/sizeof(src_array[0]);

	printf("Searching max of predefined array ...\n");
	printf("max value: %d", max_int32(src_array, length));

	return 0;
}
