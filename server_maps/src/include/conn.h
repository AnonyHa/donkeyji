#include "buffer.h"

#define MAX_CONN_SRV 32

struct _conn_client;

typedef void (*on_client_conn_cb)(struct _conn_client*);
typedef void (*on_client_read_cb)(struct _conn_client*);
typedef void (*on_client_error_cb)(struct _conn_client*);

typedef struct _conn_client {
	int srv_idx;
	int idx;//index in conn_client_mgr
	int sock;
	struct bufferevent* bev;

	buffer* wbuf;
	buffer* rbuf;
} conn_client;

typedef struct _conn_client_mgr {
	conn_client** ptr;
	size_t size;
	size_t used;
} conn_client_mgr;

typedef struct _conn_server {
	int idx;
	int listen_sock;
	int port;
	struct event* listen_ev;
	conn_client_mgr* cmgr;
	on_client_conn_cb conn_cb;
	on_client_read_cb read_cb;
	on_client_error_cb error_cb;
} conn_server;


//---------------------------------------------
//server的参数需要调用conn_server_new之后设置
conn_server* conn_server_new();
void conn_server_free(conn_server* s);
//创建listen sock，开始listen
int conn_server_startup(conn_server* s);
//---------------------------------------------


//---------------------------------------------
conn_client* conn_client_new();
void conn_client_free(conn_client* c);
void conn_client_reset(conn_client* c);
//---------------------------------------------


//---------------------------------------------
conn_client_mgr* conn_client_mgr_new();
void conn_client_mgr_free(conn_client_mgr* cm);
int conn_client_mgr_add(conn_client_mgr* cm, conn_client* c);
int conn_client_mgr_del(conn_client_mgr* cm, conn_client* c);
//---------------------------------------------


//---------------------------------------------
int conn_init();
//---------------------------------------------
