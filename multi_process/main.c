#include "master.h"
//---------------------------------------------

int fuck = 100;

int load_conf()
{}

int common_init()
{
	load_conf();
}

int main()
{
	common_init();
	master_cycle();
	return 0;
}
