typedef void (*on_client_conn_cb)(conn* c);
typedef void (*on_client_read_cb)(conn* c);
typedef void (*on_client_error_cb)(conn* c);

typedef struct _conn_server
{
	int listen_sock; 
	int port;
	struct event* listen_ev;
	conn_client_mgr* cmgr;
	on_client_conn_cb conn_cb;
	on_client_read_cb read_cb;
	on_client_error_cb error_cb;
}conn_server;

typedef struct _conn_client
{
	int idx;//index in conn_client_mgr
	int sock;
	struct bufferevent* bev;

	buffer* wbuf;
	buffer* rbuf;
}conn_client;

typedef struct _conn_client_mgr
{
	conn_client** ptr;
	size_t size;
	size_t used;
}conn_client_mgr;

//server的参数需要调用conn_server_new之后设置
conn_server* conn_server_new();
void conn_server_free(conn_server* s);

conn_client* conn_client_new();
void conn_client_free(conn_client* c);

//创建listen sock，开始listen
int conn_server_startup(conn_server* s);

conn_client_mgr* conn_client_mgr_new();
void conn_client_mgr_free(conn_client_mgr* cm);
int conn_client_mgr_add(conn_client_mgr* cm, int sock); 
int conn_client_mgr_del(conn_client* cm, conn_client* c);
