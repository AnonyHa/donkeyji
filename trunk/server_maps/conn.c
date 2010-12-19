#include "conn.h"

//max 32
static conn_server* srv_mgr[MAX_CONN_SRV];
static int srv_idx = 0;
//-----------------------------------------------------------------
//static function declaration
static void _conn_server_listen_cb(int fd, short what, void* arg);

static int _conn_client_startup(conn_client* c);
static void _conn_client_read_cb(struct bufferevent* bev, void* arg);
static void _conn_client_error_cb(struct bufferevent* bev, short what, void* arg);
//-----------------------------------------------------------------
int conn_init()
{
	int i;
	for (i=0; i<MAX_CONN_SRV; i++) {
		srv_mgr[i] = NULL;
	}
	return 0;
}

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
	//reach to the max cnt for tcp server
	if (srv_idx >= MAX_CONN_SRV) {
		log_error();
		return -1;
	}
	//add to the server mgr
	s->idx = srv_idx;
	srv_mgr[srv_idx] = s;
	srv_idx++;

	s->listen_ev = (struct event*)malloc(sizeof(struct event));
	assert(s->listen_ev);
	event_set(s->listen_ev, s->listen_sock, EV_READ|EV_PERSIST, _conn_server_listen_cb, (void*)s);
	event_add(s->listen_ev, NULL);
	return 0;
}

void
conn_server_free(conn_server* s)
{
	if (s == NULL)
		return;
	if (s->listen_ev != NULL)
		free(s->listen_ev);
	if (s->cmgr != NULL)
		conn_client_mgr_free(s->cmgr);
	free(s);
}

conn_client*
conn_server_add_client(conn_server* s, int fd)
{
	conn_client* c = conn_client_new();
	c->srv_idx = s->idx;
	c->sock = fd;

	conn_client_mgr* cm = s->cmgr;
	conn_client_mgr_add(cm, c);
	return c;
}


int
conn_server_del_client(conn_server* s, conn_client* c)
{
	conn_client_mgr* cm = s->cmgr;
	int code = conn_client_mgr_del(cm, c);
	return code;
}


//---------------------------------------------------------------
conn_client*
conn_client_new()
{
	conn_client* c = (conn_client*)calloc(1, sizeof(conn_client));
	assert(c);
	c->idx = -1;
	c->sock = -1;
	c->bev = NULL;

	c->wbuf = buffer_new();
	c->rbuf = buffer_new();
}

void
conn_client_free(conn_client* c)
{
	if (c == NULL)
		return;
	if (c->bev != NULL)
		bufferevent_free(c->bev);
	if (c->rbuf != NULL)
		buffer_free(c->rbuf);
	if (c->wbuf != NULL)
		buffer_free(c->wbuf);
	free(c);
}

void
conn_client_reset(conn_client* c)
{
	if (c == NULL)
		return;
	c->srv_idx = -1;
	c->idx = -1;
	c->sock = -1;
	bufferevent_disable(c->bev, EV_READ|EV_WRITE);
	buffer_reset(c->rbuf);
	buffer_reset(c->wbuf);
}

//--------------------------------------------------------------
conn_client_mgr*
conn_client_mgr_new()
{
	conn_client_mgr* cm = (conn_client_mgr*)calloc(1, sizeof(conn_client_mgr));
	assert(cm);
	cm->ptr = NULL;
	cm->size = 0;
	cm->used = 0;
}

void
conn_client_mgr_free(conn_client_mgr* cm)
{
	int i;
	if (cm == NULL)
		return;
	if (cm->ptr != NULL) {
		for (i=0; i<cm->size; i++) {
			if (cm->ptr[i] != NULL)
				conn_client_free(cm->ptr[i]);
		}
		free(cm->ptr);
	}
	free(cm);
}

void
conn_client_mgr_reset(conn_client_mgr* cm)
{
	if (cm ==NULL)
		return;
	int i;
	for (i=0; i<cm->used; i++)
		conn_client_reset(cm->ptr[i]);
	cm->used = 0;
}

int
conn_client_mgr_add(conn_client_mgr* cm, conn_client* c)
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

	c->idx = cm->used;

	cm->ptr[cm->used] = c;
	cm->used++;

	return 0;
}

int
conn_client_mgr_del(conn_client_mgr* cm, conn_client* c)
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

//----------------------------------------------------------------
//static function
//----------------------------------------------------------------
static int 
_conn_client_startup(conn_client* c)
{
	c->bev = bufferevent_new(c->sock, _conn_client_read_cb, NULL, _conn_client_error_cb, (void*)c);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
	return 0;
}

static void 
_conn_client_read_cb(struct bufferevent* bev, void* arg)
{
	size_t len = bev->input->off;
	if (len == 0) {
		return;
	}

	conn_client* c = (conn_client*)arg;
	char buf[len];
	bzero(buf, len);

	size_t read_len = bufferevent_read(bev, buf, len);
	if (read_len <= 0) {
		return;
	}

	buffer_append(c->rbuf, buf, read_len);

	conn_server* s = srv_mgr[c->srv_idx];

	//invoke callback, to do what the server like to do
	(s->read_cb)(c);
}

static void 
_conn_client_error_cb(struct bufferevent* bev, short what, void* arg)
{
	conn_client* c = (conn_client*)arg;

	conn_server* s = srv_mgr[c->srv_idx];

	(s->error_cb)(c);//must invoked before del client

	conn_server_del_client(s, c);
}

static void 
_conn_server_listen_cb(int fd, short what, void* arg)
{
	conn_server* s = (conn_server*)arg;
	struct sockaddr_in addr;
	int len = sizeof(addr);

	int sock = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);
	if (sock < 0) {
		log_error();
		return;
	}

	//to do:...
	int flag = fcntl(sock, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flag);

	//so nasty .......
	conn_client* c = conn_server_add_client(s, sock);

	//call back
	(s->conn_cb)(c);

	//register READ/WRITE event for client
	_conn_client_startup(c);
}
