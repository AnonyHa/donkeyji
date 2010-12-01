#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

extern int fuck;

int worker_init()
{
	//init libevent, and add listen socket event
	server_network_startup();
	log_msg(__FILE__, __LINE__, "worker init succeed");
}

int worker_cycle()
{
	worker_init();

	log_msg(__FILE__, __LINE__, "begin to event_dispath");
	event_dispatch();

	log_msg(__FILE__, __LINE__, "worker loop exit");

	return 0;
}
