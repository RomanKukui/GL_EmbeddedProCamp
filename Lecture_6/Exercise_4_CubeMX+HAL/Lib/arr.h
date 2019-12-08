/** \file 
 * \brief	Simple array tools library.
 */

#ifndef __arr_H
#define __arr_H

#include <string.h>
#include <stdio.h>

#include "usart.h"

/** \brief	Send/print arr* content to USART1 as integer values + "\n\r".
 *
 * \param[in] *arr	Pointer to array, that will be printed.
 * \param[in] size	Number of elements, that will be printed.
 *
 * \note	Used UART polling mode with HAL_MAX_DELAY timeout.
 */
void arr_print(uint32_t *arr, size_t size);

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
int8_t arr_cmp(uint32_t *arr1, uint32_t *arr2, size_t size);

/** \brief	Copy \p size elements from \p *src to \p *dst array.
 *
 * \param[out] *dst	Pointer to destination array.
 * \param[in] *src	Pointer to source array.
 * \param[in] size	Number of bytes, that will be copied.
 */
void arr_cpy(uint32_t *dst, uint32_t *src, size_t size);

#endif /* __arr_H */
