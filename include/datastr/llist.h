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
 * @file include/datastr/llist.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __UTIL_LINKED_LIST_H__
#define __UTIL_LINKED_LIST_H__

/**
 * typedef for llist:
 * Linked list header structure prototype
 * To use linked lists, create a structure
 * containing a llist_t as its first member.
 */
typedef struct llist	   llist_t;

/**
 * Iterator function prototype
 * Usage: int function_iterator(llist_t *node, void *param);
 * node is the node for which the iterator is called,
 * param is the parameter passed to llist_iterate_select
 */
typedef int (*llist_iterator_t)(llist_t *, void *);

/**
 * Linked list header structure prototype
 * To use linked lists, create a structure
 * containing a llist_t as its first member.
 */
struct llist {
	llist_t *next;
	llist_t *prev;
};

/**
 * Gets the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_get_first(llist_t *list);

/**
 * Gets the last entry in a doubly linked list
 * @param list Pointer top the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_get_last(llist_t *list);
/**
 * Gets and removes the last entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_remove_last(llist_t *list);

/**
 * Gets and removes the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_remove_first(llist_t *list);

/**
 * Adds a node to the end of the linked list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void llist_add_end(llist_t *list,llist_t *entry);

/**
 * Unlinks a node from the list it is in.
 * @param entry The node to unlink
 */
void llist_unlink(llist_t *entry);

/**
 * Initializes a new head node (list)
 * @param list Pointer to the linked list head node
 */
void llist_create(llist_t *list);

/**
 * Counts the number of nodes in a linked list
 * @param list Pointer to the linked list head node
 * @return The amount of nodes (excluding the head node)
 */
int llist_size(llist_t *list);

/**
 * Iterates over the list and calls <b>iterator</b> for
 * each node, if <b>iterator</b> returns a true condition
 * iteration will stop and the current node will be returned
 *
 * @param list		Pointer to the linked list head node
 * @param iterator	Iterator func [int func(llist _t *, void *)]
 * @param param		Parameter to the iterator. (may be NULL)
 * @return The node for which <b>iterator</b> returned true.
 */
llist_t *llist_iterate_select(	llist_t *list, 
				llist_iterator_t iterator, 
				void *param);

#endif
