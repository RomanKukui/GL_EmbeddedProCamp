/** \file
 * \brief	Dynamic list library.
 * \author	Roman Kukui
 *
 * \warning	Used high level abstraction for list item content (void*).
 *		Content type/typecasting must be controlled by user.
 *		Content memory free() controlled by user.
 *
 * \note	As List "storage" used pointer to List root.
 *		For mutable methods, such as ListAdd(), ListAddBefore(), ListRemove(),
 *		ListRemoveAddr(), used pointer for pointer. This provide proper methods
 *		feedback and empty list indication - when root pointer is NULL.
 */

#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/** \brief List item typedef
 */
typedef struct List list;

/** \brief Store dynamic List items/nodes.
 */
struct List {
	list *prev;
	void *content;
	list *next;
};

/** \brief Add list item at the end of list, as last.
 *
 * \param[in,out] root_ptr  	Address of list root pointer
 * \param[in] content_ptr 	Address of sticked content
 *
 * \return list*	Added List item address.
 *
 * \note Root pointer will be updated automatically, if item added as first.
 * \sa ListAddBefore()
 */
list* ListAdd(list **root_ptr, void *content_ptr);

/** \brief Add List item before \p before_id index.
 *
 * \param[in,out] root_ptr 	Address of list root pointer
 * \param[in] before_id		Insert before position(index)
 * \param[in] content_ptr	Address of sticked content (void*)
 *
 * \return Added List item address.
 *
 * \note Root pointer will be updated automatically, if item added as first.
 * \sa ListAdd()
 */
list* ListAddBefore(list **root_ptr, uint32_t before_id, void *content_ptr);


/** \brief Read content pointer, sticked to List item with \p index.
 *
 * \param[in] list_ptr	List root pointer
 * \param[in] index	Index of list item
 *
 * \retval void*	if present List item with \p index
 * \retval NULL		if \p index out of List range, or content not sticked
 */
void* ListIs(list *list_ptr, uint32_t index);

/** \brief Calculate number of list elements/nodes.
 *
 * \param[in] list_ptr	List root pointer
 *
 * \return Number of elements in list.
 */
uint32_t ListGetLength(list *list_ptr);

/** \brief Get List item address by specified \p index.
 *
 * \param[in] list_ptr	List root pointer
 * \param[in] index	Index of target element
 *
 * \retval list*	pointer to element with \p index in list
 * \retval NULL		if index incorrect
 */
list* ListGet(list *list_ptr, uint32_t index);

/** \brief Reach last element in list and return it address.
 *
 * \param[in] list_ptr	List root pointer.
 *
 * \retval list*	pointer to last element in list
 * \retval NULL		if list is empty
 */
list* ListGetLast(list *list_ptr);

/** \brief Remove List item by specified node address.
 *
 * \param[in,out] root_ptr	Address of List \b item pointer
 *
 * \return Pointer to item sticked content (for free() cleanup).
 * \note Modify(set as first) \p list_ptr_ptr if no previous element.
 */
void* ListRemoveAddr(list **list_ptr_ptr);

/** \brief Remove list item targeted with it's \p index.
 *
 * \param[in,out] root_ptr	Address of List root pointer
 * \param[in] index             Index of item, that will be removed
 *
 * \return Pointer to item sticked content (for free() cleanup).
 */
void* ListRemove(list **root_ptr, uint32_t index);

#endif // LIST_H_INCLUDED
