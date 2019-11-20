#include "fifo.h"

uint8_t FIFO_Init(FIFO **buff, int32_t *data_arr, uint32_t arr_size)
{
	FIFO_status status = FIFO_UNKNOWN_ERR;

	*buff = malloc(sizeof(FIFO));
	FIFO *bcs = *buff;		// bcs - buffer control structure
	if (bcs != NULL) {
		bcs->arr = data_arr;
		bcs->size = arr_size;
		bcs->count = 0;
		bcs->head_id = 0;
		bcs->tail_id = 0;

		status = FIFO_OK;
	} else {
		status = FIFO_MEMORY_ERR;
	}
	return status;
}

uint8_t FIFO_Put(FIFO *buff, int32_t data)
{
	FIFO_status status = FIFO_UNKNOWN_ERR;

	if (buff->count < buff->size) {
		buff->arr[buff->tail_id] = data;

		buff->count++;
		buff->tail_id++;
		if (buff->tail_id == buff->size) {
			buff->tail_id = 0;
		}
		status = FIFO_OK;
	} else {
		status = FIFO_FULL;
	}
	#ifdef DEBUG
	FIFO_Info(buff);
	#endif
	return status;
}

uint8_t FIFO_Get(FIFO *buff, int32_t *data)
{
	FIFO_status status = FIFO_UNKNOWN_ERR;

	if (buff->count > 0) {
		*data = buff->arr[buff->head_id];

		buff->count--;
		buff->head_id++;
		if (buff->head_id == buff->size) {
			buff->head_id = 0;
		}
		status = FIFO_OK;
	} else {
		status = FIFO_EMPTY;
	}
	#ifdef DEBUG
	FIFO_Info(buff);
	#endif
	return status;
}

uint8_t FIFO_Read(FIFO *buff, int32_t *data)
{
	FIFO_status status = FIFO_UNKNOWN_ERR;

	if (buff->count > 0) {
		*data = buff->arr[buff->head_id];
		status = FIFO_OK;
	} else {
		status = FIFO_EMPTY;
	}
	#ifdef DEBUG
	FIFO_Info(buff);
	#endif
	return status;
}

void FIFO_Error(FIFO_status status)
{
	uint8_t *msg[FIFO_RESULT_SIZE];

	msg[FIFO_EMPTY] = "FIFO empty";
	msg[FIFO_FULL] = "FIFO full";
	msg[FIFO_MEMORY_ERR] = "can't allocate memory";
	msg[FIFO_UNKNOWN_ERR] = "unknown";

	printf("ERROR: %s\n", msg[status]);
}

void FIFO_Info(FIFO *b)
{
	printf("\ncnt: %d; h: %d; t: %d; s: %d",\
		b->count, b->head_id, b->tail_id, b->size);
}
