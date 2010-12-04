#ifndef _SERVER_H
#define _SERVER_H

#include "conn.h"

struct event;

typedef struct _server
{
	int port;
	int listen_sock;
	int max_conns;

	struct event* listen_ev;
	conn_mgr* conns;
}server;

//全局的srv
extern server* srv;

void server_init();
void server_destroy();

#endif
