#include "filo.h"

uint8_t FILO_Push(FILO **filo_ptr, int32_t data)
{
	uint8_t status = FILO_ERR_UNKNOWN;

	int32_t *data_ptr = malloc(sizeof(data));
	if (data_ptr != NULL) {
		list *node = ListAdd((list**)filo_ptr, data_ptr);
		if (node == NULL) {
			// ERROR: can't create linked list item
			free(data_ptr);
			status = FILO_ERR_LIST_CREATE;
		} else {
			// update Top, without list.h modification
			// filo_ptr every time point to FILO Top
			/// \todo Create in list.h interface for RootPointer write
			*filo_ptr = node;

			*data_ptr = data;
			node->content = data_ptr;
			status = FILO_SUCCESSFUL;
		}
	} else {
		// ERROR: can't allocate memory for data
		status =  FILO_ERR_DATA_MEM_ALLOC;
	}
	return status;
}

uint8_t FILO_Pop(FILO **filo_ptr, int32_t *data)
{
	uint8_t status = FILO_ERR_UNKNOWN;

	FILO *node = *filo_ptr;
	if (node != NULL) {
		/// \todo Create in list.h, interface Get node->prev
		// store previous Node for Top pointer update
		FILO *prev_node = node->prev;

		int32_t *node_data = ListRemoveAddr((list**)filo_ptr);
		if (node_data != NULL) {
			*data = *node_data;
			free(node_data);

			status = FILO_SUCCESSFUL;
		} else {
			// ERROR: data pointer empty
                        status = FILO_ERR_NODE_EMPTY;
		}
		/// \todo Create in list.h interface for RootPointer write
		*filo_ptr = prev_node;
	} else {
		// ERROR: FILO buffer empty
		status = FILO_ERR_FILO_EMPTY;
	}
	return status;
}

uint8_t FILO_Read(FILO **filo_ptr, int32_t *data)
{
	uint8_t status = FILO_ERR_UNKNOWN;

	FILO *node = *filo_ptr;
	if (node != NULL) {
		if (node->content != NULL) {
			/// \todo Add proper interface to list.h
			*data = *((int32_t*)node->content);

			status = FILO_SUCCESSFUL;
		} else {
			// ERROR: data pointer empty
			int32_t _dummy;
			FILO_Pop(filo_ptr, &_dummy);	// remove empty node

			status = FILO_ERR_NODE_EMPTY;
		}
	} else {
		// ERROR: FILO buffer empty
		status = FILO_ERR_FILO_EMPTY;
	}
	return status;
}

void FILO_Error(FILO_Status result)
{
	uint8_t *msg[FILO_RESULT_SIZE];

	msg[FILO_ERR_DATA_MEM_ALLOC] = "Can't allocate memory for Node Data";
	msg[FILO_ERR_LIST_CREATE] = "Can't create linked list Node";
	msg[FILO_ERR_FILO_EMPTY] = "There are no any items in FILO";
	msg[FILO_ERR_NODE_EMPTY] = "Empty data pointer in Item";
	msg[FILO_ERR_UNKNOWN] = "Unknown status";

	printf("\n\nERROR: %s\n", msg[result]);
}
