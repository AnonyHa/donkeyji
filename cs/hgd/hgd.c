#include "hgd.h"
#include <event.h>

int main()
{
	int bridge_mode = -1;
	char* conf_file = NULL;
	char* log_file = NULL;

	conf_init();//read config file

	daemonize();//call daemon() as to the conf

	event_init();

	set_pwd();

	set_sig();

	set_rlimit();

	switch (bridge_mode) {
	case BRIDGE_MODE_BASE:
		bridge_init();//connect to other server
		conn_init();//server,client,connector mgr
		base_init();//set callback
		break;
	default:
		break;
	}

	event_dispatch();

	conn_close();
	log_close();
	return EXIT_SUCCESS;
}
