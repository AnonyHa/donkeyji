#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct list_t* list_new() {
	printf("list new\n");
	struct list_t* list = (struct list_t*)malloc(sizeof(struct list_t));

	if (list == NULL) {
		return NULL;
	}

	list->head = list->tail = NULL;
	list->len = 0;
	return list;
}

int list_add(struct list_t* list, struct node_t* node)
{
	printf("list add\n");
	return 0;
}

int list_del(struct list_t* list, struct node_t* node)
{
	printf("list del\n");
	return 0;
}

int list_free(struct list_t* list)
{
	return 0;
}

int list_print(struct list_t* list)
{
	return 0;
}
