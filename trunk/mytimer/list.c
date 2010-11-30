#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "timer.h"

struct list* list_new()
{
	struct list* l = (struct list*)malloc(sizeof(struct list));
	if (l == NULL)
		return NULL;
	l->head = NULL;
	l->len = 0;
	return l;
}

int list_free(struct list* l)
{
	struct timer* p;
	struct timer* n;
	if (l == NULL)
		return 0;

	p = l->head;
	while (p != NULL) {
		n = p->next;
		timer_free(p);
		p = n;
		l->len--;
	}
	return 0;
}

//插到list头
int list_insert(struct list* l, struct timer* t)
{
	if (l == NULL)
		return -1;

	t->next = l->head;

	l->head = t;
	l->len++; 

	return 0;
}

//确保t是l中的节点
int list_remove(struct list* l, struct timer* t)
{
	struct timer* p;
	struct timer* n;
	if (l == NULL || l->len == 0 || t == NULL)
		return 0;

	if (l->head == t) {//队头
		l->head = t->next;
		if (t->next != NULL)//非队尾
			t->next->prev = NULL;
	} else {
		t->prev->next = t->next;
		if (t->next != NULL)//非队尾
			t->next->prev = t->prev;
	}

	l->len--;
	return 0;
}

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
struct m_list* m_list_new()
{
	struct m_list* l = (struct m_list*)malloc(sizeof(struct m_list));
	if (l == NULL)
		return NULL;
	l->head = NULL;
	l->len = 0;
	return l;
}

int m_list_free(struct m_list* l)
{
	struct timer_wrapper* p;
	struct timer_wrapper* n;
	if (l == NULL)
		return 0;

	p = l->head;
	while (p != NULL) {
		n = p->next;
		timer_wrapper_free(p);
		p = n;
		l->len--;
	}
	return 0;
}

//插到m_list头
int m_list_insert(struct m_list* l, struct timer_wrapper* t)
{
	if (l == NULL)
		return -1;

	t->next = l->head;

	l->head = t;
	l->len++; 

	return 0;
}

//确保t是l中的节点
int m_list_remove(struct m_list* l, struct timer_wrapper* t)
{
	struct timer_wrapper* p;
	struct timer_wrapper* n;
	if (l == NULL || l->len == 0 || t == NULL)
		return 0;

	if (l->head == t) {//队头
		l->head = t->next;
		if (t->next != NULL)//非队尾
			t->next->prev = NULL;
	} else {
		t->prev->next = t->next;
		if (t->next != NULL)//非队尾
			t->next->prev = t->prev;
	}

	l->len--;
	return 0;
}

