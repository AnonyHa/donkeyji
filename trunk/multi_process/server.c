#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>

#include "conf.h"
#include "server.h"
#include "conn.h"
#include "log.h"


static void _server_handle_listen(int fd, short ev, void* arg);
static int _server_add_conn(conn* c);
static int _server_create_listen_sock(int port);
//----------------------------------------
server* srv = NULL;

int server_init()
{
	srv = (server*)calloc(1, sizeof(server));
	srv->port = cfg->port;
	srv->max_fds = cfg->max_fds;
	srv->listen_sock = _server_create_listen_sock(srv->port);
	srv->listen_ev = (struct event*)calloc(1, sizeof(struct event));
	if (srv->listen_sock < 0)
		return -1;

	srv->conns = conn_mgr_new();

	event_init();

	log_msg(__FILE__, __LINE__, "server obj created");

	return 0;
}

int server_destroy()
{}

int server_network_register() 
{
	log_msg(__FILE__, __LINE__, "begin to init libevent");
	//event_init();//libevent init
	event_set(srv->listen_ev, srv->listen_sock, EV_READ|EV_PERSIST, _server_handle_listen, srv->listen_ev);
	event_add(srv->listen_ev, NULL);
	log_msg(__FILE__, __LINE__, "listen_ev add to libevent");
	return 0;
}


static void _server_handle_listen(int fd, short ev, void* arg)
{
	log_msg(__FILE__, __LINE__, "a connection comes, pid=%d", getpid());
	if (srv->conns->used > srv->max_fds)
		return;
	struct sockaddr_in addr;
	int len = sizeof(addr);
	int sock = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);
	if (sock < 0) {
		perror("accept");
		log_msg(__FILE__, __LINE__, "accept failed");
		return;
	}

	int flag = fcntl(sock, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flag);

	conn* c = conn_new(sock);

	int ret = _server_add_conn(c);
	if (ret < 0) {
		log_msg(__FILE__, __LINE__, "add connection to server failed");
		conn_free(c);
		return;
	}
	log_msg(__FILE__, __LINE__, "add connection succeed");
}

static int _server_add_conn(conn* c)
{
	return conn_mgr_add(srv->conns, c);
}

static int _server_create_listen_sock(int port)
{
	int sock;
	int flag;
	struct sockaddr_in addr;
	if (port < 0) {
		return -1;
	} 
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		return -1;
	}

	flag = fcntl(sock, F_GETFL, 0);
	if (flag == -1) {
		return -1;
	}
	if (fcntl(sock, F_SETFL, flag | O_NONBLOCK) < 0) {
		return -1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		return -1;
	} 
	if (bind(sock, (struct sockaddr* )&addr, sizeof(addr))<0) {
		return -1;
	} 
	if (listen(sock, 128) < 0) {
		return -1;
	}

	return sock;
}
