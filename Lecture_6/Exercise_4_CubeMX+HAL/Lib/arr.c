#include "arr.h"

void arr_print(uint32_t *arr, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		uint8_t msg[15];
		sprintf((char*)msg, "%i\t", arr[i]);
		HAL_UART_Transmit(&huart1, msg, strlen((char*)msg), HAL_MAX_DELAY);
	}
	uint8_t ret_msg[] = {"\n\r"};
	HAL_UART_Transmit(&huart1, ret_msg, 2, HAL_MAX_DELAY);
}

/** \brief	Compare elements of two arrays.
 *
 * \param[in] *arr1	Pointer to array #1.
 * \param[in] *arr2	Pointer to array #2.
 * \param[in] size	Number of elements for comparing (size of array).
 *
 * \note 	Will be compared only \p size number of elements.
 *
 * \retval -1		Arrays elements identical.
 * \retval 		Index of first different element.
 */
int8_t arr_cmp(uint32_t *arr1, uint32_t *arr2, size_t size)
{
	int8_t ret_val = -1;		// default value, arrays identical
	
	for (size_t i = 0; i < size; i++) {
		if (arr1[i] != arr2[i]) {
			ret_val = i;
			break;
		}
	}
	return ret_val;
}

/** \brief	Copy \p size elements from \p *src to \p *dst array.
 *
 * \param[out] *dst	Pointer to destination array.
 * \param[in] *src	Pointer to source array.
 * \param[in] size	Number of bytes, that will be copied.
 */
void arr_cpy(uint32_t *dst, uint32_t *src, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		dst[i] = src[i];
	}
}
