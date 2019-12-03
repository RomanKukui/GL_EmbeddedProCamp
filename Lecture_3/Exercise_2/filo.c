#include "filo.h"

uint8_t FILO_Push(FILO **filo_ptr, int32_t data)
{
	uint8_t status = FILO_UNKNOWN_ERR;

	int32_t *data_ptr = malloc(sizeof(data));
	if (data_ptr != NULL) {
		list *node = ListAdd((list**)filo_ptr, data_ptr);
		if (node == NULL) {
			// ERROR: can't create linked list item
			free(data_ptr);
			status = FILO_LIST_CREATE_ERR;
		} else {
			// update Top, without list.h modification
			// filo_ptr every time point to FILO Top
			/// \todo Create in list.h interface for RootPointer write
			*filo_ptr = node;

			*data_ptr = data;
			node->content = data_ptr;
			status = FILO_OK;
		}
	} else {
		// ERROR: can't allocate memory for data
		status =  FILO_DATA_MEM_ALLOC_ERR;
	}
	return status;
}

uint8_t FILO_Pop(FILO **filo_ptr, int32_t *data)
{
	uint8_t status = FILO_UNKNOWN_ERR;

	FILO *node = *filo_ptr;
	if (node != NULL) {
		/// \todo Create in list.h, interface Get node->prev
		// store previous Node for Top pointer update
		FILO *prev_node = node->prev;

		int32_t *node_data = ListRemoveAddr((list**)filo_ptr);
		if (node_data != NULL) {
			*data = *node_data;
			free(node_data);

			status = FILO_OK;
		} else {
			// ERROR: data pointer empty
                        status = FILO_NODE_EMPTY;
		}
		/// \todo Create in list.h interface for RootPointer write
		*filo_ptr = prev_node;
	} else {
		// ERROR: FILO buffer empty
		status = FILO_FILO_EMPTY;
	}
	return status;
}

uint8_t FILO_Read(FILO **filo_ptr, int32_t *data)
{
	uint8_t status = FILO_UNKNOWN_ERR;

	FILO *node = *filo_ptr;
	if (node != NULL) {
		if (node->content != NULL) {
			/// \todo Add proper interface to list.h
			*data = *((int32_t*)node->content);

			status = FILO_OK;
		} else {
			// ERROR: data pointer empty
			int32_t _dummy;
			FILO_Pop(filo_ptr, &_dummy);	// remove empty node

			status = FILO_NODE_EMPTY;
		}
	} else {
		// ERROR: FILO buffer empty
		status = FILO_FILO_EMPTY;
	}
	return status;
}

void FILO_Error(FILO_Status result)
{
	uint8_t *msg[FILO_RESULT_SIZE];

	msg[FILO_DATA_MEM_ALLOC_ERR] = "can't allocate mem. for Data";
	msg[FILO_LIST_CREATE_ERR] = "can't create list Node";
	msg[FILO_FILO_EMPTY] = "FILO empty";
	msg[FILO_NODE_EMPTY] = "item data empty";
	msg[FILO_UNKNOWN_ERR] = "unknown status";

	printf("ERROR: %s\n", msg[result]);
}
