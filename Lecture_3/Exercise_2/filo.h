/** \file
 *
 * \brief FILO (stack) Linked List based buffer.
 *
 * \warning 	Used linked list library, as a data container.
 *		When used tiny payload (for example int8_t) can cause memory
 *		fragmentation.
 * \pre		Use lib.h from Lecture 2 Exercise 3.
 */

#ifndef FILO_H_INCLUDED
#define FILO_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#include "..\..\Lecture_2\Exercise_3\list.h"

/// List type encapsulated into new FILO buffer type.
typedef list FILO;

/// Status codes, returned by FILO library methods.
enum FILO_RESULT {
	FILO_OK,	//!< Successful operation
	FILO_DATA_MEM_ALLOC_ERR,//!< Can't allocate memory for Node Data
        FILO_LIST_CREATE_ERR,	//!< Can't create linked list Node
        FILO_FILO_EMPTY,	//!< There are no any Nodes in FILO buffer
        FILO_NODE_EMPTY,	/**< There are empty pointer to Data in list Node
				\warning !Empty Node was removed! */
        FILO_UNKNOWN_ERR,	//!< Unknown result status (used as initial)
        FILO_RESULT_SIZE
};

/// Returned by FILO status codes type
typedef enum FILO_RESULT FILO_Status;

uint8_t FILO_Push(FILO **filo_ptr, int32_t data);
/**< \brief 	Push \p data in Top of FILO buff.
 *
 * \param[in,out] filo_ptr 	\b Address of FILO Top Pointer
 * \param[in] data		Item, that will be pushed into Top of FILO
 *
 * \return 	Calling result according to FILO_RESULT
 */

uint8_t FILO_Pop(FILO **filo_ptr, int32_t *data);
/**< \brief	Pop data from Top of the FILO buffer. Remove item from FILO.
 *
 * \param[in,out] filo_ptr	\b Address of FILO Top Pointer
 * \param[out] data		Pointer to data receiving variable
 *
 * \return 	Calling result according to FILO_RESULT
 * \note	Remove item from FILO buffer.
 */

uint8_t FILO_Read(FILO **filo_ptr, int32_t *data);
/**< \brief	Read data from Top of FILO.
 *
 * \param[in,out] filo_ptr	\b Address of FILO Top Pointer
 * \param[out] data		Pointer to data receiving variable
 *
 * \return 	Calling result according to FILO_RESULT
 * \note	Do not remove item with data, only read it.
 * \warning	Item without payload will be removed.
 */

void FILO_Error(FILO_Status result);
/**< \brief 	Process error codes and output text Msg to stdout
 *
 * \param[in] result 	Status, that will be printed
 */

#endif // FILO_H_INCLUDED
