/** @file
 * GL EmbeddedProCamp - Task#3
 *
 * Create queue library based on array:
 *	- int Put(int);		// add element
 *	- int Get(*int);	// read element and remove from queue
 *	- Int Read(*int);	// read element without removing it from stack
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include "fifo.h"

//#define DEBUG

int main()
{
	FIFO_status status = FIFO_UNKNOWN_ERR;

	int32_t data_arr[5];
	FIFO *fifo_buff;
	status = FIFO_Init(&fifo_buff, data_arr, 5);

	uint32_t tmp = 0;
	while (1) {
		printf("[+] put, [-] get, [r] read < ");

		uint8_t ch = 0;
		ch = getch();

		switch (ch) {
	case '+':
		tmp = 0;
		printf("put: ");
		scanf("%d", &tmp);

		status = FIFO_Put(fifo_buff, tmp);
		if (status != FIFO_OK) {
			FIFO_Error(status);
		}
		break;

	case '-':
		status = FIFO_Get(fifo_buff, &tmp);
		if (status != FIFO_OK) {
			FIFO_Error(status);
		} else {
			printf("get: %d\n", tmp);
		}
		break;

	case 'r':
		status = FIFO_Read(fifo_buff, &tmp);
		if (status != FIFO_OK) {
			FIFO_Error(status);
		} else {
			printf("read: %d\n", tmp);
		}
		}
	}

	return 0;
}
