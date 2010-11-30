//全局，唯一的数据，单件
struct huge_server** server_list;
struct huge_client* client_list[SERVER_POOL_SIZE];
struct huge_connector* connector_list[CONNECTOR_POOL_SIZE];
int client_list_size;
int total_clients;
int max_clients;

int conn_init()//初始化conn_obj
{}

int conn_close()//清理conn_obj
{}
