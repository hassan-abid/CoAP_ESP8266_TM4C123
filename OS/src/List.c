/*
 * @file 	list.c
 * @author	Hassan Abid
 * @date	Feb 7, 2018
 *
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include "List.h"





/**
 * @defgroup list_Private_Functions	list Private Functions
 * @{
 */

/**
 *
 * @param list
 * @param index
 * @param item
 */
void insert(List_t* list, ListItem_t* index, ListItem_t* item)
{
	if (list->end == NULL)
	{
		list->end = item;
		item->next = item;
		item->prev = item;

		//Initialize iterator when the
		List_ResetIterator(list);
	}
	else
	{
		item->next = index->next;
		item->prev = index;
		item->prev->next = item;
		item->next->prev = item;
	}

	item->list = list;
	list->size++;
}


/** @}*/

/**
 * @defgroup list_Public_Functions	list Public Functions
 * @{
 */

/**
 *
 * @param list
 */
void List_Init(List_t* list)
{
	list->index = NULL;
	list->end = NULL;
	list->size = 0;
}

/**
 *
 * @param list
 * @param item
 */
void List_InsertSorted(List_t* list, ListItem_t* item)
{
	ListItem_t* iter;
	uint32_t count;

	if (list->end == NULL)
	{
		insert(list, NULL, item);
		return ;
	}

	//iterate the list until the desired index
	count = 0;
	for (	iter = list->end->next ;
			iter->value > item->value && count < list->size;
			iter = iter->next)
	{
		count++;
	}

	//insert the item in the list before the item
	//whose value is lower.
	if (iter->prev == list->end)
	{
		if (list->end->value > item->value)
			list->end = item;
	}
	insert(list, iter->prev, item );

}

/**
 *
 * @param list
 * @param item
 */
void List_InsertEnd(List_t* list, ListItem_t* item)
{
	insert(list, list->end, item);
	list->end = item;
}



/**
 *
 * @param list
 * @param item
 */
void List_Remove(List_t* list, ListItem_t* item)
{
	if (list->size == 0) return;

	if (list->size == 1)
	{
		list->end = NULL;
		list->index = NULL;
	}
	else
	{
		item->prev->next = item->next;
		item->next->prev = item->prev;
		
		if (item  == list->end)
			list->end = item->prev;

		if (item == list->index)
			list->index = item->prev;
	}

	item->next = NULL;
	item->prev = NULL;
	item->list = NULL;

	list->size--;

}


/**
 *
 * @param list
 * @return
 */
ListItem_t* List_Iterate(List_t* list)
{
	if (list->end == NULL)
		return NULL;
	list->index = list->index->next;
	return list->index;

}

/**
 *
 * @param list
 */
void List_ResetIterator(List_t* list)
{
	list->index = list->end;
}
/** @}*/
