conn_server* srv_mgr = NULL; 
conn_client* clt_mgr = NULL;

static void _conn_server_listen_cb(int fd, short what, void* arg);
static void _conn_server_read_cb(struct bufferevent* bev, void* arg);
static void _conn_server_error_cb(struct bufferevent* bev, short what, void* arg);

conn_server* conn_server_new()
{}

int conn_server_start(conn_server* s) 
{
	event_set(s->listen_ev, EV_READ|EV_PERSIST, _conn_server_listen_cb, NULL);
	event_add(s->listen_ev, NULL);
}
