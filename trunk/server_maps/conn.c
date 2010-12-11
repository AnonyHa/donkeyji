#include "conn.h"

conn_server* srv_mgr = NULL; 
conn_client* clt_mgr = NULL;

//-----------------------------------------------------------------
//static function declaration
static void _conn_server_listen_cb(int fd, short what, void* arg);
static void _conn_server_read_cb(struct bufferevent* bev, void* arg);
static void _conn_server_error_cb(struct bufferevent* bev, short what, void* arg);

static int _conn_client_startup(conn* c)
static void _conn_client_read_cb(struct bufferevent* bev, void* arg);
static void _conn_client_error_cb(strut bufferevent* bev, short what, void* arg);
//-----------------------------------------------------------------

conn_server* conn_server_new()
{
	conn_server* s = (conn_server*)calloc(1, sizeof(conn_server));
	assert(s);
	return s;
}

int conn_server_startup(conn_server* s)
{
	event_set(s->listen_ev, EV_READ|EV_PERSIST, _conn_server_listen_cb, s);
	event_add(s->listen_ev, NULL);
}

static void _conn_server_listen_cb(int fd, short what, void* arg)
{
	int fd = accept(fd);

	//to do:...
	set_nonblocking(fd);

	conn_client* c = conn_client_new();
	c->sock = fd;

	conn_server* s = (conn_server*)arg;

	//call back
	s->on_conn_cb(c);

	//register READ/WRITE event for client
	_conn_client_startup(c);
}

static int _conn_client_startup(conn* c)
{
	c->bev = bufferevent_new(c->fd, _conn_client_read_cb, NULL, _conn_client_error_cb, (void*)c);
	bufferevent_enable(c->bev);
	return 0;
}

static void _conn_client_read_cb(struct bufferevent* bev, void* arg)
{}

static void _conn_client_error_cb(strut bufferevent* bev, short what, void* arg)
{}
