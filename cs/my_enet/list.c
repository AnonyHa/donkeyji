#include "enet/list.h"

void enet_list_clear(ENetList* list)
{
	//sentinel属于非链表中的有效数据节点，sentinel->previous指向最后一个节点,sentinel->next指向头节点
	//初始化指向自己
	list->sentinel.next = &list->sentinel;
	list->sentinel.previous = &list->sentinel;
}

void* enet_list_remove(ENetListIterator position)
{
	position->previous->next = position->next;
	position->next->previous = position->previous;
	return position;
}

ENetListIterator enet_list_insert(ENetListIterator position, void* data)
{
	ENetListIterator result = (ENetListIterator)data;
	result->previous = position->previous;
	result->next = position;

	result->previous->next = result;
	position->previous = result;

	return result;
}

size_t enet_list_size(ENetList* list)
{
	size_t size = 0;
	ENetListIterator position;

	for (position = enet_list_begin(list);
	     position != enet_list_end(list);
	     position = enet_list_next(position)) {
		size++;
	}
}
