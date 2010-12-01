#include "server.h"

server* server_init()
{
	server* srv = (server*)calloc(1, sizeof(server));
	conf_init();
	log_init();

	event_init();
	event_set(&srv->listen_ev);
	event_add(&srv->listen_ev, NULL);

	return srv;
}
