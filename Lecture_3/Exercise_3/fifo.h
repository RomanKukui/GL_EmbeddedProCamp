/** \file
 * \brief Array-based FILO buffer library
 *
 * \author	Roman Kukui roman.kukui@gmail.com
 */


#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

/// Store main FIFO parameters and data array pointer.
struct FIFO_Control {
	uint32_t count;		///< Number of data items in buffer
	uint32_t head_id;	///< Define head position in \p arr (cell with data)
	uint32_t tail_id;	///< Define position of tail (empty cell)
	uint32_t size;		///< Size of \p arr
	int32_t *arr;		///< Pointer to array, where stored FIFO data
};

/// Type shortcut for FIFO control structure
typedef struct FIFO_Control FIFO;

/// Status codes, returned by library methods.
enum FIFO_RESULT {
	FIFO_OK,		///< Successful operation
	FIFO_EMPTY,		///< FIFO don't have information
	FIFO_FULL,		///< FIFO data \p arr is full
	FIFO_MEMORY_ERR,	///< Control structure mem. allocation error
	FIFO_UNKNOWN_ERR,	///< Unknown result status (used as initial)
	FIFO_RESULT_SIZE
};

/// Type shortcut for FIFO return codes
typedef enum FIFO_RESULT FIFO_status;


uint8_t FIFO_Init(FIFO **buff, int32_t *data_arr, uint32_t arr_size);
/**< \brief	Initialize FIFO control structure.
 *
 * \param[in,out] buff 	\b Empty pointer to FIFO control structure
 * \param[in] data_arr 	Pointer to array for storing items
 * \param[in] arr_size	Size of \p data_arr
 *
 * \return 	Calling result according to FIFO_status
 * \note	Control structure will be dynamically allocated by FIFO_Init().
 */

uint8_t FIFO_Put(FIFO *buff, int32_t data);
/**< \brief
 *
 * \param buff FIFO*
 * \param data uint32_t
 * \return uint8_t
 *
 */

uint8_t FIFO_Get(FIFO *buff, int32_t *data);
/**< \brief
 *
 * \param buff FIFO*
 * \param data uint32_t*
 * \return uint8_t
 *
 */

uint8_t FIFO_Read(FIFO *buff, int32_t *data);
/**< \brief
 *
 * \param buff FIFO*
 * \param data uint32_t*
 * \return uint8_t
 *
 */

void FIFO_Error(FIFO_status status);
/**< \brief
 *
 * \param status FIFO_Status
 * \return void
 *
 */

#endif // FIFO_H_INCLUDED
