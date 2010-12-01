#include "conn.h"

conn* conn_new(int sock)
{
	conn* c = (conn*)calloc(1, sizeof(conn));
	c->fd = sock;
	//c->bev
}

//----------------------
conn_mgr* conn_mgr_new()
{
	conn_mgr* cm = (conn_mgr*)calloc(1, sizeof(conn_mgr));
	cm->ptr = NULL;
	cm->size = 0;
	cm->used = 0;
	return cm;
}
