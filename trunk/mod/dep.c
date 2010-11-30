#include <stdio.h>
#include "dep.h"

static int dep_stat; 

int dep_init()
{
	printf("dep init\n");
	return 0;
}

int dep_run()
{
	printf("dep run\n"); 
	return 0;
}

int dep_exit()
{
	printf("dep exit");
	return 0;
}
