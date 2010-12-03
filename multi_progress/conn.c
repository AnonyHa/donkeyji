#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <event.h>

#include "conn.h"
#include "log.h"
#include "chunk.h"

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

	//足够大的缓冲
	conn* con = (conn*)arg;//connection对象
	size_t len = 1024;
	char buf[len];
	bzero(buf, len);

	size_t read_len = bufferevent_read(bev, buf, len);
	if (read_len <= 0) {
		log_msg(__FILE__, __LINE__, "bufferevent read error: %s", strerror(errno));
		return;
	}
	chunk* ck = chunkqueue_get_append_chunk(conn->read_q);	
	if (ck == NULL) 
		return;
	chunk_append(ck, buf, read_len);

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
	/*在构造函数里的malloc失败如何处理比较好????*/
	conn* c = (conn*)calloc(1, sizeof(conn));
	assert(c);
	c->fd = sock;
	c->bev = bufferevent_new(sock, _conn_handle_read, NULL, _conn_handle_err, (void*)c);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);

	c->read_q = chunkqueue_new();
	c->write_q = chunkqueue_new();

	log_msg(__FILE__, __LINE__, "register connection to libevent: sock = %d", sock);
	return c;
}

void conn_free(conn* c)
{
	if (c == NULL)
		return;

	if (c->bev != NULL)
		bufferevent_free(c->bev);

	if (c->read_q != NULL)
		chunkqueue_free(c->read_q);
	if (c->write_q != NULL)
		chunkqueue_free(c->write_q);

	free(c);
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

void conn_mgr_free(conn_mgr* cm)
{}

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
