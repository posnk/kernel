/******************************************************************************\
Copyright (C) 2015 Peter Bosch
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file datastr/llist.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "datastr/llist.h"

#include <stddef.h>
#include <assert.h>

/**
 * Gets the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_get_first(llist_t *list)
{
	assert(list != NULL);
	return list->next;
}

/**
 * Gets the last entry in a doubly linked list
 * @param list Pointer top the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_get_last(llist_t *list)
{
	assert(list != NULL);
	return list->prev;
}

/**
 * Gets and removes the last entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_remove_last(llist_t *list)
{
	llist_t *result;
	assert(list != NULL);
	result = list->prev;
	if (result == list)
		return NULL;
	llist_unlink(result);
	return result;
}

/**
 * Gets and removes the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_remove_first(llist_t *list)
{
	llist_t *result;
	assert(list != NULL);
	result = list->next;
	if (result == list)
		return NULL;
	llist_unlink(result);
	return result;
}

/**
 * Adds a node to the end of the linked list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void llist_add_end(llist_t *list,llist_t *entry)
{
	assert(list != NULL);
	assert(entry != NULL);
	entry->prev = list->prev;
	entry->next = list;
	list->prev->next = entry;
	list->prev = entry;
}

/**
 * Unlinks a node from the list it is in.
 * @param entry The node to unlink
 */
void llist_unlink(llist_t *entry)
{
	assert(entry != NULL);
	assert(  (entry->prev && entry->next) || 
		((!entry->prev) && (!entry->next)));
	if (entry->prev)
		entry->prev->next = entry->next;
	if (entry->next)
		entry->next->prev = entry->prev;
	entry->next = 0;
	entry->prev = 0;
}

/**
 * Initializes a new head node (list)
 * @param list Pointer to the linked list head node
 */
void llist_create(llist_t *list)
{
	assert(list != NULL);
	list->next = list;
	list->prev = list;
}

/**
 * Counts the number of nodes in a linked list
 * @param list Pointer to the linked list head node
 * @return The amount of nodes (excluding the head node)
 */
int llist_size(llist_t *list)
{
	llist_t *work;
	int size = 0;
	assert(list != NULL);
	work = list->next;
	for (;;){
		assert (work != NULL);
		if (list == work)
			break;
		size++;
		work = work->next;
	}
	return size;
}

/**
 * Iterates over the list and calls <b>iterator</b> for
 * each node, if <b>iterator</b> returns a true condition
 * iteration will stop and the current node will be returned
 *
 * @param list		Pointer to the linked list head node
 * @param iterator	An iterator [int func(llist_t *, void *).]
 * @param param		Parameter to the iterator. (may be NULL)
 * @return The node for which <b>iterator</b> returned true.
 */
llist_t *llist_iterate_select(	llist_t *list, 
				llist_iterator_t iterator, 
				void *param)
{
	llist_t *work;
	assert(iterator != NULL);
	assert(list != NULL);
	work = list->next;
	for (;;) {
		assert (work != NULL);
		if (list == work)
			break;
		if (iterator(work, param))
			return work;
		work = work->next;
	}
	return NULL;
}

