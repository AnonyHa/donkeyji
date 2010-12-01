#ifndef _CONN_H
#define _CONN_H

#include <sys/types.h>

struct buffer;
typedef struct _conn
{
	int fd;
	struct bufferevent* bev;
	//buffer* rbuf;
	//buffer* wbuf;
}conn;

conn* conn_new(int sock);
int conn_free(conn* c);

//-------------------------------------
typedef struct _conn_mgr
{
	conn** ptr;
	size_t size;
	size_t used;
}conn_mgr;

conn_mgr* conn_mgr_new();
int conn_mgr_free(conn_mgr* cm);
int conn_mgr_add(conn_mgr* cm,  conn* c);
int conn_mgr_del(conn_mgr* cm, conn* c);
#endif
