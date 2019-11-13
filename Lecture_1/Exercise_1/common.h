///@file

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdint.h>

/*!
 * Swap 2 \b bytes (1,2 -> 2,1).
 *
 * @note 	Bits position(in byte) will not be changed.
 * @param[in] 	input	2 bytes for swapping.
 * @return	Swapped variable.
 */
uint16_t swap16(uint16_t input);

/*!
 * Swap 4 \b bytes (1,2,3,4 -> 4,3,2,1).
 *
 * @note	Bits position(in byte) will not be changed.
 * @param[in]	input	4 bytes for swapping.
 * @return	Swapped/reversed value.
 */
uint32_t swap32(uint32_t input);

/*!
 * Swap 8 \b bytes (1-8 -> 8-1).
 *
 * @note	Bits position(in byte) will not be changed.
 * @param[in]	input	8 bytes for swapping.
 * @return	Swapped/reversed value.
 */
 uint64_t swap64(uint64_t input);

#endif // COMMON_H_INCLUDED
