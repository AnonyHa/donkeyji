#include "extend.h"


int main()
{
	struct extend* e = extend_new();
	e->f(e);
	return 0;
}
