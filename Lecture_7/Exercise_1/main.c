/** \file
 * \author Roman Kukui
 *
 * Task:
 * Create project using OS Protothreads to schedule the two
 * or more threads.
 */


#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "pt/pt.h"

/** \brief 	Polling/dummy delay function.
 *
 * \param[in] delay	Delay period in \b milliseconds.
 *
 * \pre		Include <time.h> header for clock() system call.
 */
void delay(uint32_t delay)
{
	delay += clock();
	while(clock() < delay) {
	}
}

/// Threads control structures
static struct pt thr_1, thr_2, thr_input;

/// Threads flags(dummy "semaphores") (0 - frozen, 1 - executed).
static uint8_t f_1, f_2;

/// Program exit flag (0 - executing, 1 - exit program).
static uint8_t f_quit;

/** \brief 	Protothreads wrapped Thread 1 code.
 *
 * \param[in,out] *pt	Address of thread control structure.
 */
static PT_THREAD(thread_1(struct pt *pt))
{
	PT_BEGIN(pt);
	printf("%d: thread 1 started\n", clock());

	while (1) {
		printf("%d: thread 1 running\n", clock());
		delay(500);	// user action simulation

		f_1 = 0;	// freeze
		printf("%d: thread 1 frozen\n", clock());

		PT_WAIT_UNTIL(pt, (f_1 == 1));	// !* do not block main loop

		printf("%d: thread 1 resumed\n", clock());
		delay(750);	// user action
	}
	printf("%d: thread 1 ended\n", clock());
	PT_END(pt);
}

/** \brief 	Protothreads wrapped Thread 2 code.
 *
 * \param[in,out] *pt	Address of thread control structure.
 */
static PT_THREAD(thread_2(struct pt *pt))
{
	PT_BEGIN(pt);
	printf("%d: thread 2 started\n", clock());

	while (1) {
		printf("%d: thread 2 running\n", clock());
		delay(750);	// user action

		f_2 = 0;	// freeze
		printf("%d: thread 2 frozen\n", clock());

		PT_WAIT_WHILE(pt, (f_2 == 0));	// !* do not block main loop

		printf("%d: thread 2 resumed\n", clock());
		delay(500);	// user action
	}
	PT_END(pt);
}

/** \brief 	Protothreads wrapped user input thread code.
 *
 * \param[in,out] *pt	Address of thread control structure.
 */
static PT_THREAD(thread_input(struct pt *pt))
{
	// Buffer for user input char
	// used static for preventing store in stack
	static uint8_t input_ch = 0;

	PT_BEGIN(pt);

	input_ch = getch();

	switch (input_ch) {
	case '1':
		f_1 = (f_1 == 1) ? 0 : 1;
		break;

	case '2':
		f_2 = (f_2 == 1) ? 0 : 1;
		break;

	case 'q':
		f_quit = 1;
		break;
	}
	PT_END(pt);
}

/// Main loop
int main()
{
	f_quit = 0;

	printf("%d: main started\n", clock());
	printf("\t>> for changing threads state pres '1' or '2', 'q' for exit\n");

	PT_INIT(&thr_1);
	PT_INIT(&thr_2);

	PT_INIT(&thr_input);

	while (1) {
		thread_1(&thr_1);
		thread_2(&thr_2);

		thread_input(&thr_input);

		if (f_quit == 1) {
			break;
		}
	}
	return 0;
}
