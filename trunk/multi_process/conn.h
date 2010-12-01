#ifndef _CONN_H
#define _CONN_H

struct buffer;
typedef struct _conn
{
	int fd;
	struct bufferevent bev;
	buffer* rbuf;
	buffer* wbuf;
}conn;

typedef struct _conn_mgr
{
	conn** ptr;
	size_t size;
	size_t used;
}conn_mgr;

#endif
