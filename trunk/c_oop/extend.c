#include <stdio.h>
#include <stdlib.h>

#include "extend.h"

//纯虚函数在子类中的实现
static int extend_f(struct base_head* p)
{
	struct extend* pe = (struct extend*)p;
	printf("%d\n", pe->ext);
}

//构造函数的同时，将纯虚函数实例化
struct extend* extend_new()
{
	struct extend* e = (struct extend*)calloc(1, sizeof(struct extend));
	e->ext = 1;
	e->f = extend_f;
	return e;
}

