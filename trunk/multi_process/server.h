#ifndef _SERVER_H
#define _SERVER_H

#include "conn.h"

struct event;

typedef struct _server
{
	int port;
	int listen_sock;
	int max_fds;

	struct event* listen_ev;
	conn_mgr* conns;
}server;

int server_init();
int server_network_init();

#endif
