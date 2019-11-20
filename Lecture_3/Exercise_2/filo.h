/** \file
 *
 * \brief FILO (stack) Linked List based buffer.
 *
 * \warning 	Used linked list library, as a data container.
 *		When used tiny payload (for example uint32_t) can cause memory
 *		fragmentation.
 */

#ifndef FILO_H_INCLUDED
#define FILO_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#include "..\..\Lecture_2\Exercise_3\list.h"

/// List type encapsulated into new FILO buffer type.
typedef list FILO;

/// Returned by FILO status codes type
typedef enum FILO_RESULT FILO_Status;

/// Status codes, returned by FILO library methods.
enum FILO_RESULT {
	FILO_SUCCESSFUL,	//!< Successful operation
	FILO_ERR_DATA_MEM_ALLOC,//!< Can't allocate memory for Node Data
        FILO_ERR_LIST_CREATE,	//!< Can't create linked list Node
        FILO_ERR_FILO_EMPTY,	//!< There are no any Nodes in FILO buffer
        FILO_ERR_NODE_EMPTY,	/**< There are empty pointer to Data in list Node
				\warning !Empty Node was removed! */
        FILO_ERR_UNKNOWN,	//!< Unknown result status (used as initial)
        FILO_RESULT_SIZE
};

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

/** \brief 	Process error codes and output text Msg to stdout
 *
 * \param[in] result 	Status, that will be printed
 */
void FILO_Error(FILO_Status result);

#endif // FILO_H_INCLUDED
