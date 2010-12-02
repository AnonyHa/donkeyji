#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <event.h>

#include "conn.h"
#include "log.h"

static void _conn_handle_read(struct bufferevent* bev, void* arg)
{
	log_msg(__FILE__, __LINE__, "data from conn to read: fd = %d", ((conn*)arg)->fd);

	/*
	size_t len = bev->input->off;
	if (len == 0) {
		log_msg(__FILE__, __LINE__, "no data");
		return;
	}
	*/

	size_t len = 1024;//must be enough to receive
	char buf[len];
	bzero(buf, len);

	size_t read_len = bufferevent_read(bev, buf, len);
	if (read_len <= 0) {
		log_msg(__FILE__, __LINE__, "bufferevent read error: %s", strerror(errno));
		return;
	}

	log_msg(__FILE__, __LINE__, "read data: %s", buf);
}


static void _conn_handle_err(struct bufferevent* bev, short what, void* arg)
{
	log_msg(__FILE__, __LINE__, "conn error: fd = %d", ((conn*)arg)->fd);
	bufferevent_free(bev);
	close((int)arg);
}


conn* conn_new(int sock)
{
	conn* c = (conn*)calloc(1, sizeof(conn));
	assert(c);
	c->fd = sock;
	c->bev = bufferevent_new(sock, _conn_handle_read, NULL, _conn_handle_err, (void*)c);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
	log_msg(__FILE__, __LINE__, "register connection to libevent: sock = %d", sock);
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
	assert(cm);
	cm->size = 0;
	cm->used = 0;
	cm->ptr = NULL;
	return cm;
}

int conn_mgr_add(conn_mgr* cm, conn* c)
{
	int i;
	if (cm->size == 0) {
		cm->size += 128;
		cm->ptr = (conn**)malloc(cm->size * sizeof(conn*));
		assert(cm->ptr);
	} else if (cm->size == cm->used) {
		cm->size += 128;
		cm->ptr = realloc(cm->ptr, sizeof(conn*) * cm->size);
		assert(cm->ptr);
	}
	cm->ptr[cm->used] = c;
	cm->used++;
	return 0;
}

int conn_mgr_del(conn_mgr* cm, conn* c)
{}
