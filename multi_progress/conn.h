#ifndef _CONN_H
#define _CONN_H

#include <mp.h>

#include "chunk.h"

struct buffer;
typedef struct _conn
{
	int fd;
	struct bufferevent* bev;
	chunkqueue* read_q;
	chunkqueue* write_q;
}conn;

conn* conn_new(int sock);
void conn_free(conn* c);

//-------------------------------------
//Êý×éÈÝÆ÷
typedef struct _conn_mgr
{
	conn** ptr;
	size_t size;
	size_t used;
}conn_mgr;

conn_mgr* conn_mgr_new();
void conn_mgr_free(conn_mgr* cm);
int conn_mgr_add(conn_mgr* cm,  conn* c);
int conn_mgr_del(conn_mgr* cm, conn* c);
#endif
