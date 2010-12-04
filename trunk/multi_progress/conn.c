#include <mp.h>

#include "server.h"
#include "conn.h"
#include "log.h"
#include "chunk.h"

static void conn_process_chunk(conn* c);
static void _conn_handle_read(struct bufferevent* bev, void* arg);
static void _conn_handle_err(struct bufferevent* bev, short what, void* arg);

//---------------------------------------------------

static conn* 
conn_new()
{
	/*在构造函数里的malloc失败如何处理比较好????*/
	conn* c = (conn*)calloc(1, sizeof(conn));
	assert(c);
	c->fd = -1;//这里也不设置fd
	c->ndx = -1;//表示还没有放入conn_mgr

	c->bev = NULL;//这里不注册事件

	c->read_q = chunkqueue_new();
	c->write_q = chunkqueue_new();

	return c;
}

static void
conn_free(conn* c)
{
	if (c == NULL)
		return;

	if (c->bev != NULL)
		bufferevent_free(c->bev);

	if (c->read_q != NULL)
		chunkqueue_free(c->read_q);

	if (c->write_q != NULL)
		chunkqueue_free(c->write_q);

	close(c->fd);

	free(c);
}

//设置fd
static void 
conn_set_fd(conn* c, int fd)
{
	c->fd = fd;
}

//为conn注册read/write事件
static void 
conn_register_event(conn* c)
{
	c->bev = bufferevent_new(c->fd, _conn_handle_read, NULL, _conn_handle_err, (void*)c);
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
}

//删除为conn注册的事件
static void 
conn_unregister_event(conn* c)
{
	bufferevent_free(c->bev);
}


//重置，而不是释放内存
static void
conn_reset(conn* c)
{
	if (c == NULL)
		return;

	conn_unregister_event(c);//这里不能reset，只好free掉，注册的时候再malloc了，悲剧。。。

	c->fd = -1;		
	c->ndx = -1;

	if (c->read_q != NULL)
		chunkqueue_reset(c->read_q);

	if (c->write_q != NULL)
		chunkqueue_reset(c->write_q);

	close(c->fd);
}

//---------------------------------------------------
conn_mgr* 
conn_mgr_new()
{
	conn_mgr* cm = (conn_mgr*)calloc(1, sizeof(conn_mgr));
	assert(cm);
	cm->size = 0;
	cm->used = 0;
	cm->ptr = NULL;
	return cm;
}

void 
conn_mgr_free(conn_mgr* cm)
{
	int i;
	if (cm == NULL)
		return;

	if (cm->ptr != NULL) {
		for (i=0; i<cm->used; i++) {
			conn_free(cm->ptr[i]);
		}
		free(cm->ptr);
	}

	free(cm);
}

//会预先分配，以128的速度增长
void 
conn_mgr_add_conn(conn_mgr* cm, int sock)
{
	conn* c;
	int i;
	if (cm->size == 0) {//第一次分配
		cm->size = 128;
		cm->ptr = (conn**)calloc(cm->size, sizeof(conn*));
		for (i=0; i<cm->size; i++) {
			cm->ptr[i] = conn_new();
		}
	} else if (cm->size == cm->used) {//之前分配的用完了
		cm->size += 128;	
		cm->ptr = (conn**)realloc(cm->ptr, cm->size);
		for (i=cm->used; i<cm->size; i++) {
			cm->ptr[i] = conn_new();
		}
	}
	c = cm->ptr[cm->used];
	c->ndx = cm->used;
	cm->used++;

	conn_set_fd(c, sock);
	conn_register_event(c);

}

//
static void 
conn_mgr_del_conn(conn_mgr* cm, conn* c)
{
	int ndx = c->ndx;
	conn* tmp;

	if (ndx == -1)//不在数组里
		return;

	if (ndx == cm->used - 1) {//数组最尾的
		c->ndx = -1;
		conn_reset(c);//以备下次使用
		return;
	}

	//交换数组尾和要删除的那个conn
	tmp = c;
	cm->ptr[ndx] = cm->ptr[cm->used-1];
	cm->ptr[ndx]->ndx = ndx;
	cm->ptr[cm->used-1] = tmp;
	cm->ptr[cm->used-1]->ndx = -1;//标示为未使用

	//删除read/write事件，置sock = -1
	conn_reset(c);

	return;
}

//----------------------------------------------------------
static void 
_conn_handle_read(struct bufferevent* bev, void* arg)
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
	conn* c = (conn*)arg;//connection对象
	size_t len = 1024;
	char buf[len];
	bzero(buf, len);

	size_t read_len = bufferevent_read(bev, buf, len);
	if (read_len <= 0) {
		log_msg(__FILE__, __LINE__, "bufferevent read error: %s", strerror(errno));
		return;
	}
	chunk* ck = chunkqueue_get_append_chunk(c->read_q);	
	if (ck == NULL) 
		return;
	buffer_append(ck->mem, buf, read_len);

	log_msg(__FILE__, __LINE__, "read data: %s", buf);

	conn_process_chunk(c);
}


static void 
_conn_handle_err(struct bufferevent* bev, short what, void* arg)
{
	conn* c = (conn*)arg;//connection对象
	log_msg(__FILE__, __LINE__, "conn error: fd = %d", c->fd);
	//---------------
	//use global srv
	//---------------
	conn_mgr_del_conn(srv->conns, c);
	log_msg(__FILE__, __LINE__, "close sock: Fd = %d", c->fd);
}

static void 
conn_process_chunk(conn* c)
{}
