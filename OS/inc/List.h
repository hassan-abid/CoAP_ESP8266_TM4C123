/*
 * @file 	list.h
 * @author	Hassan Abid
 * @date	Feb 7, 2018
 *
 * 
 */

#ifndef INC_LIST_H_
#define INC_LIST_H_

struct List_t;

typedef struct ListItem_t{

	struct ListItem_t* next;		///pointer to next item in list
	struct ListItem_t* prev;		///pointer to previous item in list (doubly linked list)

	uint32_t value;			/// Order variable. Used to sort the list.

	void* parent;			/// Pointer to the object.
	struct List_t* list;	/// Pointer to the list of which this node is a part of.

}ListItem_t;


typedef struct List_t{

	uint32_t size;
	ListItem_t* index;
	ListItem_t* end;

}List_t;


#define List_getElement(item)							((item)->parent)
#define List_getNextElement( item )				List_getElement((item)->next)
#define List_getPreviousElement( item )		List_getElement((item)->prev)
#define List_getFirstElement( list )			List_getElement((list)->end->next)
#define List_getSize( list )							((list)->size)

#define List_RemoveFirstElement( list ) 	List_Remove( (list), List_getFirstElement( (list) ) )


#ifdef __cplusplus
extern "C"{
#endif 
 
void List_Init(List_t* list);
void List_InsertSorted(List_t* list, ListItem_t* item);
void List_InsertEnd(List_t* list, ListItem_t* item);
void List_Remove(List_t* list, ListItem_t* item);

ListItem_t* List_Iterate(List_t* list);
void List_ResetIterator(List_t* list);

#ifdef __cplusplus
}
#endif


#endif /* INC_LIST_H_ */
