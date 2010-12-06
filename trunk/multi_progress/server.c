#include <mp.h>

#include "conf.h"
#include "server.h"
#include "conn.h"
#include "log.h"


static void _server_handle_listen(int fd, short ev, void* arg);
static int _server_create_listen_sock(int port);
//----------------------------------------
server* srv = NULL;

void 
server_init()
{
	log_msg(__FILE__, __LINE__, "server init");

	srv = (server*)calloc(1, sizeof(server));
	assert(srv);

	srv->port = cfg->port;
	srv->max_conns = cfg->max_conns;

	srv->listen_sock = _server_create_listen_sock(srv->port);

	srv->listen_ev = (struct event*)calloc(1, sizeof(struct event));
	assert(srv->listen_ev);

	srv->conns = conn_mgr_new();
	assert(srv->conns);

	//��ʼ��libevent
	event_init();

	log_msg(__FILE__, __LINE__, "server obj created");
}

void 
server_destroy()
{}

int 
server_network_register() 
{
	log_msg(__FILE__, __LINE__, "begin to init libevent");
	event_set(srv->listen_ev, srv->listen_sock, EV_READ|EV_PERSIST, _server_handle_listen, srv->listen_ev);
	event_add(srv->listen_ev, NULL);
	log_msg(__FILE__, __LINE__, "listen_ev add to libevent");
	return 0;
}


static void 
_server_handle_listen(int fd, short ev, void* arg)
{
	log_msg(__FILE__, __LINE__, "a connection comes, pid=%d", getpid());
	log_msg(__FILE__, __LINE__, "used = %d, max = %d, pid=%d", srv->conns->used, srv->max_conns, getpid());
	//��α�֤??
	if (srv->conns->used > srv->max_conns) {
		log_msg(__FILE__, __LINE__, "connections full%d");
		return;
	}

	struct sockaddr_in addr;
	int len = sizeof(addr);
	int sock = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);
	if (sock < 0) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		return;
	}

	int flag = fcntl(sock, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flag);

	//��Ҫ�Ľ����޸�Ϊ�ڴ�ط�ʽ����һ��connection�ĳ�
	conn_mgr_add_conn(srv->conns, sock);

	log_msg(__FILE__, __LINE__, "add connection succeed");
}

static int 
_server_create_listen_sock(int port)
{
	int sock;
	int flag;
	struct sockaddr_in addr;
	if (port < 0) {
		log_msg(__FILE__, __LINE__, "wrong port");
		exit(1);
	} 
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	}

	flag = fcntl(sock, F_GETFL, 0);
	if (flag == -1) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	if (fcntl(sock, F_SETFL, flag | O_NONBLOCK) < 0) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	} 
	if (bind(sock, (struct sockaddr* )&addr, sizeof(addr))<0) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	} 
	if (listen(sock, 128) < 0) {
		log_msg(__FILE__, __LINE__, strerror(errno));
		exit(1);
	}

	return sock;
}
