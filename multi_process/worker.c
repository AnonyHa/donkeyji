#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

extern int fuck;

int worker_init()
{
	//init libevent, and add listen socket event
	server_network_startup();
}

int worker_cycle()
{
	worker_init();

	event_dispatch();

	return 0;
}
