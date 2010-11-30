#include <stdio.h>
#include <stdlib.h>
#include "mod.h"
#include "list.h"

//-----------------------------------
static int mod_a_stat = 0;
static struct list_t* list = NULL;
//-----------------------------------

static int mod_a_init();
static int mod_a_run();

struct mod_t mod_a = {
	mod_a_init,
	mod_a_run,
	NULL
};


static int mod_a_init()
{
	printf("mod a init\n");
	list = list_new();
	if (list == NULL)
		return -1;
	return 0;
}

static int mod_a_run()
{
	mod_a_stat++;
	printf("mod_a_stat = %d\n", mod_a_stat);

	struct node_t* node = (struct node_t*)malloc(sizeof(struct node_t));
	node->data = &mod_a_stat;
	list_add(list, node);
	return 0;
}
