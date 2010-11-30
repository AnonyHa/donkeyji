#include <stdio.h>
#include <stdlib.h>

#include "extend.h"

static int extend_f(struct base_head* p)
{
	struct extend* pe = (struct extend*)p;
	printf("%d\n", pe->ext);
}

struct extend* extend_new()
{
	struct extend* e = (struct extend*)calloc(1, sizeof(struct extend));
	e->ext = 1;
	e->f = extend_f;
	return e;
}

