#include <stdio.h>
#include "mod.h"

static int mod_b_init();
static int mod_b_run();

static int mod_b_stat;

struct mod_t mod_b = {
	mod_b_init,
	mod_b_run,
	NULL
};

static int mod_b_init()
{
	printf("mod b init\n");
	return 0;
}

static int mod_b_run()
{
	mod_b_stat--;
	printf("mod b run\n");
	printf("mod_b_stat = %d\n", mod_b_stat);
	return 0;
}
