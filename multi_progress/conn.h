#ifndef _CONN_H
#define _CONN_H

#include <mp.h>

#include "chunk.h"

struct buffer;
typedef struct _conn
{
	int fd;
	int ndx;//��¼��conn_mgr�е��±�
	struct bufferevent* bev;
	chunkqueue* read_q;
	chunkqueue* write_q;
}conn;

//-------------------------------------
//��������
typedef struct _conn_mgr
{
	conn** ptr;
	size_t size;
	size_t used;
}conn_mgr;

conn_mgr* conn_mgr_new();
void conn_mgr_free(conn_mgr* cm);
void conn_mgr_reset(conn_mgr* cm);
void conn_mgr_add_conn(conn_mgr* cm, int sock);
#endif
