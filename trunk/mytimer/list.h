#ifndef _LIST_H
#define _LIST_H

struct timer;

struct list {
	struct timer* head;
	int len;
};

struct m_list {
	struct timer_wrapper* head;
	int len;
};

struct list* list_new();
int list_free(struct list* l);

int list_append(struct list* l, struct timer* t);
int list_remove(struct list* l, struct timer* t);


struct m_list* m_list_new();
int m_list_free(struct m_list* ml);
int m_list_append(struct m_list* ml, struct timer_wrapper* mt);
int m_list_remove(struct m_list* ml, struct timer_wrapper* mt);

#endif
