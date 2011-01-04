//------------------------------------------
//client
//------------------------------------------
struct conn_client
{};

//------------------------------------------
//server
//------------------------------------------
struct conn_server
{};

//------------------------------------------

void listen_callback()
{
	int fd = accept();
	conn_client* c = conn_client_new(fd);

	bufferevent_new(fd);
	bufferevent_enable();

	int vfd = gen_vfd();
	client_mgr[vfd] = c;
}

//----------------------------------------
struct rpc_method;
extern std::map<int, rpc_method*> rpc_map;
//----------------------------------------

void conn_read_callback(int fd, short event, void* arg)
{
	int vfd = (int)arg;
	conn_client* c = client_mgr[vfd];

	char buf[1024];
	bufferevent_read(buf);

	//解析msg头，找出msg id和msg len
	int msg_id = ...;
	int msg_len = ...;

	//根据msg id找到对应的rpc
	rpc_method = rpc_map[msg_id];

	MethodDescriptor* method = rpc_method->method;
	//req用来unpack出msg中的request结构体
	TestRequest* req = rpc_method->req->New();
	//rsp用来pack向client发送响应
	TestResponse* rsp = rpc_method->rsp->New();
	//unpack出request
	req->ParseFromString(buf);

	//成员函数指针
	Closure* done = NewCallBack(rpc_method, done_callback);
	//CallMethod里会调用到Echo，Echo里调用到done->Run()
	//done->Run()里调用rpc_method->*method()
	rpc_method->svc->CallMethod(method, NULL, req, rsp, done);
}

void conn_error_callback()
{}

int create_listen_sock()
{}

struct event* listen_event;
void server_init()
{
	int listen_sock = create_listen_sock();

	listen_event = new struct event;
	event_set(listen_event, callback);
	event_add(listen_event, NULL);
}
