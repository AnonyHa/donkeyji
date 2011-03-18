#define SERVER_POOL_SIZE 8//1024//没必要这么大
#define CONNECTOR_POOL_SIZE 8//1024//没必要这么大

struct huge_server {
	int srv_id;
	unsigned short srv_port;
	unsigned int srv_addr;
	int srv_fd;
	struct sockaddr_in srv_addr;
	struct event srv_listen_ev;//for listen
	unsigned int srv_start_time;//start time

	void (*srv_data_callback)(int, void*, unsigned int);
	void (*srv_conn_callback)(int);
	void (*srv_close_callback)(int);
};
struct huge_server* conn_tcp_server_new();
void conn_tcp_server_free(struct huge_server* srv);
//----------------------------------------------------

struct huge_client {
	int clt_id;
	int srv_id;
	unsigned short clt_port;
	unsigned int clt_addr;
	int clt_fd;
	struct bufferevent clt_rw_ev;//for read/write
	struct huge_buff clt_rbuf;//buffer for read
	struct huge_buff clt_wbuf;//buffer for write
	unsigned int clt_conn_time;//connected time
	unsigned int clt_last_hb_time;//last heartbeat time
};
struct huge_client* conn_client_new();
void conn_client_free(struct huge_client* clt);
//----------------------------------------------------

struct huge_connector {
	int conn_id;
	unsigned short conn_port;
	unsigned int conn_addr;
	struct sockaddr_in conn_addr;
	int conn_fd;
	struct event conn_rw_ev;//for read/write
	struct huge_buff clt_rbuf;//buffer for read
	struct huge_buff clt_wbuf;//buffer for write
	unsigned int conn_conn_time;

	void (*conn_data_callback)(void*, unsigned int);
	void (*conn_conn_callback)();
	void (*conn_close_callback)();
};
struct huge_connector* conn_tcp_connector_new();
void conn_tcp_connector_free(struct huge_connector* conn);
