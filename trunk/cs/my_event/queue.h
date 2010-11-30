#ifndef _QUEUE_H
#define _QUEUE_H

//用宏来操作链表
#define TAILQ_FIRST(head)	((head)->tqh_first)
#define TAILQ_END(head)	NULL
#define TAILQ_NEXT(elm, field)	((elm)->field.tqe_next)

#define TAILQ_EMPTY(head)	(TAILQ_FIRST(head)) == TAILQ_END(head)


#define TAILQ_INIT(head) do {	\
	(head)->tqh_first = NULL;	\
	(head)->tqh_last = &(head)->tqh_first;	\
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do {	\
	(elm)->field.tqe_next = NULL;	\
	(elm)->field.tqe_prev = (head)->tqh_last;	\
	*(head)->tqh_last = (elm);	\
	(head)->tqh_last = &(elm)->field.tqe_next;	\
} while(0)

#define TAILQ_REMOVE(head, elm, field) do {	\
	if ((elm)->field.tqe_next != NULL)	\
		(elm)->field.tqe_next->field.tqe_prev =	(elm)->field.tqe_prev;	\
	else	\
		(head)->tqh_last = (elm)->field.tqe_prev;	\
	*(elm)->field.tqe_prev = (elm)->field.tqe_next;	\
} while(0)


#endif
