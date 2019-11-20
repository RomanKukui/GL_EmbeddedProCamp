/** @file
 * GL EmbeddedProCamp - Task#2
 *
 * Create stack library based on linked list:
 *	- int Push(int);	// add element
 *	- int Pop(*int);	// read element and remove from stack
 *	- int Read(*int);	// read element without removing it from stack
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include "filo.h"

int main()
{
	FILO *filo_buff = NULL;

	int32_t tmp = 0;
	FILO_Status status = FILO_ERR_UNKNOWN;

	while (1) {
		printf("\n[+] push, [-] pop, [r] read < ");

		uint8_t ch = 0;
		ch = getch();

		switch (ch) {
	case '+':
		tmp = 0;
		printf("push: ");
		scanf("%d", &tmp);

		status = FILO_Push(&filo_buff, tmp);
		if (status != FILO_SUCCESSFUL) {
			FILO_Error(status);
		}
		break;

	case '-':
		status = FILO_Pop(&filo_buff, &tmp);
		if (status != FILO_SUCCESSFUL) {
			FILO_Error(status);
		} else {
			printf("pop: %d\n", tmp);
		}
		break;

	case 'r':
		status = FILO_Read(&filo_buff, &tmp);
		if (status != FILO_SUCCESSFUL) {
			FILO_Error(status);
		} else {
			printf("read: %d", tmp);
		}
		}
	}
	return 0;
}
