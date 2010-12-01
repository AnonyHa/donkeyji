#ifndef _SERVER_H
#define _SERVER_H

typedef struct _server
{
	int port;
	int srv_sock;
	int max_fds;
	int used_fds;
	struct event listen_ev;
	conn_mgr* conns;
}server;

server* server_init();

#endif
