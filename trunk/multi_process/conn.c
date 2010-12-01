#include <stdio.h>
#include <stdlib.h>

#include <event.h>

#include "conn.h"

static void _conn_handle_read(struct bufferevent* bev, void* arg)
{}

static void _conn_handle_err(struct bufferevent* bev, int what, void* arg)
{}


conn* conn_new(int sock)
{
	conn* c = (conn*)calloc(1, sizeof(conn));
	c->fd = sock;
	c->bev = bufferevent_new(sock, _conn_handle_read, NULL, _conn_handle_err, NULL);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
	log_msg(__FILE__, __LINE__, "register connection to libevent");
	return c;
}

int conn_free(conn* c)
{
	if (c == NULL)
		return 0;
	if (c->bev != NULL)
		bufferevent_free(c->bev);
	free(c);
	return 0;
}
//----------------------------------------------------------
conn_mgr* conn_mgr_new()
{
	conn_mgr* cm = (conn_mgr*)calloc(1, sizeof(conn_mgr));
	cm->ptr = NULL;
	cm->size = 0;
	cm->used = 0;
	return cm;
}

int conn_mgr_add(conn_mgr* cm, conn* c)
{}

int conn_mgr_del(conn_mgr* cm, conn* c)
{}
