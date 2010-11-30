#include "afxdb.h"
#include <stdlib.h>

int main()
{
	int ret;
	ret = db_init();
	if (ret < 0)
		exit(-1);

	db_run();

	return 0;
}
