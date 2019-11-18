#include "list.h"

list* ListAdd(list **root_ptr, void *content_ptr)
{
        list *list_ptr = *root_ptr;

	list *curr_item = (list*)malloc(sizeof(list));
	curr_item->content = content_ptr;

	if (list_ptr == NULL) {
		curr_item->prev = NULL;
		curr_item->next = NULL;

		*root_ptr = curr_item;	// update root by first
	} else {
		list_ptr = ListGetLast(list_ptr);

		curr_item->prev = list_ptr;
		curr_item->next = list_ptr->next;

		list_ptr->next = curr_item;
	}
	return curr_item;
}

list* ListAddBefore(list **root_ptr, uint32_t before_id, void *content_ptr)
{
	list *list_ptr = ListGet(*root_ptr, before_id);

	if (list_ptr != NULL) {
		list *curr_item = (list*)malloc(sizeof(list));
		curr_item->content = content_ptr;

		if (before_id == 0) {
			*root_ptr = curr_item;		// set item as first
		} else {
			list_ptr->prev->next = curr_item;
		}
		curr_item->prev = list_ptr->prev;
		curr_item->next = list_ptr;
		list_ptr->prev = curr_item;

		return curr_item;
	}
	return NULL;		// unreachable before_id
}

uint32_t ListGetLength(list *list_ptr)
{
	uint32_t counter = 0;

	do {
		counter++;
		list_ptr = list_ptr->next;
	} while (list_ptr != NULL);

	return counter;
}

list* ListGetLast(list *list_ptr)
{
	while (list_ptr->next != NULL) {
		list_ptr = list_ptr->next;
	}
	return list_ptr;
}

list* ListGet(list *list_ptr, uint32_t index)
{
	uint32_t i = 0;
	while(list_ptr != NULL) {
		if (i == index) {
			return list_ptr;
		} else {
			list_ptr = list_ptr->next;
			i++;
		}
	}
	return NULL;		// unexpected end of list
}

void* ListIs(list *list_ptr, uint32_t index)
{
	list_ptr = ListGet(list_ptr, index);

	if (list_ptr != NULL) {
		return list_ptr->content;
	} else {
		return NULL;
	}
}

void* ListRemoveAddr(list **list_ptr_ptr)
{
	list *list_ptr = *list_ptr_ptr;

	if (list_ptr->next != NULL) {
		list_ptr->next->prev = list_ptr->prev;
	}
	if (list_ptr->prev != NULL) {
		list_ptr->prev->next = list_ptr->next;
	} else {
		// first element, change root list pointer
		*list_ptr_ptr = list_ptr->next;
	}
	if ((list_ptr->prev == NULL) && (list_ptr->next == NULL)) {
		// last element, terminate root list pointer
		*list_ptr_ptr = NULL;
	}
	void *tmp_content_ptr = list_ptr->content;

	free(list_ptr);

	return tmp_content_ptr;
}

void* ListRemove(list **root_ptr, uint32_t index)
{
	list *del_ptr = ListGet(*root_ptr, index);

	if (del_ptr != NULL) {
		void *content = ListRemoveAddr(&del_ptr);

		if (index == 0) {
			*root_ptr = del_ptr;
		}
		return content;
	}
	return NULL;		// no item with this index
}

static void ListItemPrint(list *item_ptr)
{
	printf("> item: %08X (", item_ptr);
	printf("prev: %08X", item_ptr->prev);
	printf("  content: %08X", item_ptr->content);
	printf("  next: %08X)\n", item_ptr->next);
}

static void ListPrint(list *list_ptr)
{
	printf("\nList root addr: %08X\n", list_ptr);
	while (list_ptr != NULL) {
		ListItemPrint(list_ptr);
		list_ptr = list_ptr->next;
	}
}
