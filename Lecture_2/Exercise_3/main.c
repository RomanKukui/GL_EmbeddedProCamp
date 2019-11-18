#include <stdio.h>
#include <stdlib.h>

#include "list.h"

// array of pointers
uint8_t *menu1_text[] = {
	"Item 1",
	"Item 2",
	"Item 3",
	"Item 4",
	"Item 5"
};

uint8_t *menu2_text[] = {
	"Point 1",
	"Point 2",
	"Point 3",
	"Point 4"
};

// can use multiple lists with single library
list *menu1_list = NULL;
list *menu2_list = NULL;


void menu_init(list **root_ptr, uint8_t *menu[], uint8_t items)
{
	for (uint8_t i = 0; i < items; i++) {
		ListAdd(root_ptr, menu[i]);
	}
}

void menu_print(list *root_ptr)
{
	while (root_ptr != NULL) {
		if (root_ptr->content == NULL) {
			return;
		}
		printf("\t%s\n", (char*)root_ptr->content);
		root_ptr = root_ptr->next;
	}
	putchar('\n');
}

void get_target(uint8_t *list, uint8_t *item)
{
	printf("\nPlease select <list #>-<item #> (ex 1-2):\n");
	*list = 0;
	*item = 0;

	do {
		scanf("%d-%d", list, item);
		getchar();	// flush input buffer from Enter/return

		if (*list < 1) {
			printf("Invalid list number. Try again ...\n");
		}
		if (*item < 1) {
			printf("Invalid item number. Try again ...\n");
		}
	} while ((*list < 1) || (*item < 1));
}

void menu_add(void)
{
	uint8_t list = 1;
	uint8_t item = 1;
	get_target(&list, &item);

	item--;

	uint8_t *content = malloc(32);
	printf("Enter content for item (! without spaces !):\n");
	scanf("%31s", content);
	getchar();		// flush input buffer from Enter

	switch (list) {
	case 1:
		if (menu1_list == NULL) {
			ListAdd(&menu1_list, content);
		} else {
			ListAddBefore(&menu1_list, item, content);
		}
		break;
	case 2:
		if (menu2_list == NULL) {
			ListAdd(&menu2_list, content);
		} else {
			ListAddBefore(&menu2_list, item, content);
		}
		break;
	}
}

void menu_remove(void)
{
	uint8_t list = 0;
	uint8_t item = 0;
	get_target(&list, &item);

	item--;

	switch (list) {
	case 1:
		printf("Remove content: %s\n", (char*)ListIs(menu1_list, item));
		// free() not work with predefined
		//free(ListRemove(&menu1_list, item));
		ListRemove(&menu1_list, item);
		break;
	case 2:
		printf("Remove content: %s\n", (char*)ListIs(menu2_list, item));
		// free() not work with predefined
		//free(ListRemove(&menu2_list, item));
		ListRemove(&menu2_list, item);
		break;
	}
}

int main()
{
	menu_init(&menu1_list, menu1_text, 5);
	menu_init(&menu2_list, menu2_text, 4);

	while (1) {
		printf("\nPlease select action:\n");
		printf("a - insert item before; r - remove item; p - print all lists; q - quit\n\n");

		uint8_t ch = getchar();
		getchar();		// flush input buffer from Enter

		switch (ch) {
		case 'a':	// add
			menu_add();
			break;
		case 'r':	// remove
			menu_remove();
			break;
		case 'p':	// print all
			printf("List #1\n");
			menu_print(menu1_list);

			printf("\nList #2\n");
			menu_print(menu2_list);
			break;
		case 'q':
			return 0;
		default:
			printf("Unknown command. Try again ...");
		}
	}
	return 0;
}
