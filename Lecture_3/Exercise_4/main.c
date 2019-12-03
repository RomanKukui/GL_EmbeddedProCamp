/** @file
 * GL EmbeddedProCamp - Task#4
 *
 * Create function, that receives pointer to other function as a parameter.
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>

/** \brief Menu item action callback terminator
 */
void item1_action(void)
{
	printf("action in item 1\n");
}

/** \brief Menu item action callback terminator
 */
void item2_action(void)
{
	printf("action in item 2\n");
}

/** \brief Menu item action callback terminator
 */
void item3_action(void)
{
	printf("action in item 3\n");
}


/** \brief Dummy delay for \p delay in ms.
 *
 * \param[in] delay 	Delay time in ms.
 */
void delay(uint32_t delay)
{
	delay += clock();
	while(clock() < delay) {
	}
}

/** \brief 	Simulate some 2s action with callback after it
 *
 * \param[in] *func	Pointer to function, that was called after delay
 */
void delay_with_callback(void (*func)(void))
{
	delay(2000);		// simulate some action
	(*func)();		// call callback
}

/** \brief	Simulate some general final action.
 */
void after_delay(void)
{
	printf("Delay completed. Second general action\n\n");
}

int main()
{
	// describe menu items
	uint8_t *menu[3];
	menu[0] = "Menu item 1";
	menu[1] = "Menu item 2";
	menu[2] = "Menu item 3";

	// initialize menu callbacks
	void (*menu_action[3])(void);
	menu_action[0] = item1_action;
	menu_action[1] = item2_action;
	menu_action[2] = item3_action;

	while (1) {
		for (uint8_t i = 0; i < 3; i++) {
			printf("%s\n", menu[i]);
		}
		printf("select menu item < ");

		uint32_t num = 0;
		scanf("%d", &num);

		if ((num >= 1) && (num <= 3)) {
			num--;				// convert to index
			(*menu_action[num])();

			delay_with_callback(after_delay);
		}
	}
	return 0;
}
