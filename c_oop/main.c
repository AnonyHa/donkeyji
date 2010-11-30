#include "extend.h"


int main()
{
	data_integer* e = data_integer_new();
	e->free(e);
	return 0;
}
