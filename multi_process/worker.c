#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

extern int fuck;

int worker_init()
{
	server_init_network();
}

int worker_cycle()
{
	worker_init();

	event_dispatch();

	return 0;
}
