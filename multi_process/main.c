#include "conf.h"
#include "log.h"
#include "server.h"
#include "master.h"
//---------------------------------------------


int main()
{
	conf_init();
	log_init();
	server_init();

	master_cycle();

	return 0;
}
