#include <stdio.h>
#include <stdlib.h>

#include "extend.h"

//纯虚函数在子类中的实现
static int data_integer_free(struct data_base* p)
{
	struct data_integer* pe = (struct data_integer*)p;
	free(pe->buffer);
	free(pe);
	//free(p);
}

//构造函数的同时，将纯虚函数实例化
struct data_integer* data_integer_new()
{
	struct data_integer* e = (struct data_integer*)calloc(1, sizeof(struct data_integer));
	e->value = 0;

	//纯虚函数实例化	
	e->free = data_integer_free;

	return e;
}
