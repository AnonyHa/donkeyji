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

conn_server* 
conn_server_new()
{
	//use calloc to set the memory to zero
	conn_server* s = (conn_server*)calloc(1, sizeof(conn_server));
	assert(s);
	s->listen_ev = NULL;
	s->listen_sock = -1;
	s->port = -1;
	s->conn_cb = NULL;
	s->read_cb = NULL;
	s->error_cb = NULL;
	return s;
}

int
conn_server_startup(conn_server* s)
{
	s->listen_ev = (struct event*)malloc(sizeof(struct event));
	assert(s->listen_ev);
	event_set(s->listen_ev, EV_READ|EV_PERSIST, _conn_server_listen_cb, s);
	event_add(s->listen_ev, NULL);
}

static void 
_conn_server_listen_cb(int fd, short what, void* arg)
{
	conn_server* s = (conn_server*)arg;
	struct sockaddr_in addr;
	int len = sizeof(addr);

	int fd = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);
	if (fd < 0) {
		log_error();
		return;
	}

	//to do:...
	int flag = fcntl(fd, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);

	conn_client_mgr_add(s->cmgr, fd);

	//call back
	s->on_conn_cb(c);

	//register READ/WRITE event for client
	_conn_client_startup(c);
}

void conn_server_free(conn_server* s)
{}

conn_client* conn_client_new()
{
	conn_client* c = (conn_client*)calloc(1, sizeof(conn_client));
	assert(c);
	c->idx = -1;
	c->sock = -1;
	c->bev = NULL;

	c->wbuf = buffer_new();
	c->rbuf = buffer_new();
}

void conn_client_free(conn_client* c)
{
	if (c == NULL)
		return;
	bufferevent_free(c->bev);
	buffer_free(c->rbuf);
	buffer_free(c->wbuf);
	free(c);
}

void conn_client_reset(conn_client* c)
{
	if (c == NULL)
		return;
	c->idx = -1;
	c->sock = -1;
	bufferevent_disable(c->bev, EV_READ|EV_WRITE);
	buffer_reset(c->rbuf);
	buffer_reset(c->wbuf);
}

conn_client_mgr* conn_client_mgr_new()
{
	conn_client_mgr* cm = (conn_client_mgr*)calloc(1, sizeof(conn_client_mgr));
	assert(cm);
	cm->ptr = NULL;
	cm->size = 0;
	cm->used = 0;
}

void conn_client_mgr_free(conn_client_mgr* cm)
{
	if (cm == NULL)
		return;
	int i;
	for (i=0; i<size; i++)
		conn_client_free(cm->ptr[i]);
	free(cm->ptr);
	free(cm);
}

void conn_client_mgr_reset(conn_client_mgr* cm)
{
	if (cm ==NULL)
		return;
	int i;
	for (i=0; i<cm->used; i++)
		conn_client_reset(cm->ptr[i]);
	cm->used = 0;
}

int conn_client_mgr_add(conn_client_mgr* cm, int sock)
{
	int i;
	if (cm->size == 0) {
		cm->size += 128;
		cm->ptr = (conn_client**)calloc(cm->size, sizeof(conn_client*));
		for (i=0; i<cm->size; i++) {
			cm->ptr[i] = conn_client_new();
		} 
	} else if (cm->size == cm->used) {
		cm->size += 128;
		cm->ptr = (conn_client**)realloc(cm->ptr, cm->size * sizeof(conn_client*));
		for (i=cm->used; i<cm->size; i++) {
			cm->ptr[i] = conn_client_new();
		}
	}

	conn_client* c = conn_client_new();
	c->sock = sock;
	c->idx = cm->used;

	cm->ptr[cm->used] = c;
	cm->used++;

	return 0;
}

int conn_client_mgr_del(conn_client_mgr* cm, conn_client* c)
{
	conn_client* tmp;
	if (c->idx == -1)
		return 0;
	if (c->idx == cm->used-1) {
		conn_client_reset(cm->ptr[cm->used]);
	} else {
		tmp = c;	
		cm->ptr[c->idx] = cm->ptr[cm->used-1];
		cm->ptr[c->idx]->idx = c->idx;
		cm->ptr[cm->used-1] = tmp;
		conn_client_reset(cm->ptr[cm->used-1]);
	}
	cm->used--;
	return 0;
}
//------------------------------------------------------------------------
static int 
_conn_client_startup(conn* c)
{
	c->bev = bufferevent_new(c->fd, _conn_client_read_cb, NULL, _conn_client_error_cb, (void*)c);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
	return 0;
}

static void 
_conn_client_read_cb(struct bufferevent* bev, void* arg)
{}

static void 
_conn_client_error_cb(strut bufferevent* bev, short what, void* arg)
{}
