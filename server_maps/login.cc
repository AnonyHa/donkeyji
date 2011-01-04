#include "conf.h"
#include "typedef.h"
#include "conn.h"
#include "login.h"

//------------------------------------------
//static function declaration
static void _login_client_conn(conn_client* c);
static void _login_client_read(conn_client* c);
static void _login_client_error(conn_client* c);

static void _login_realm_reg(conn_client* c);
static void _login_realm_data(conn_client* c);
static void _login_realm_unreg(conn_client* c);

static void conn_client_send(conn_client* c);
static session* _conn_get_session(long uid);
//------------------------------------------
lua_State* gL = NULL;

int
login_init()
{
	svc_login_init();//protocol init

	//lua init
	gL = luaL_newstate();
	luaL_openlibs(gL);
	//luaopen_luasql_mysql(gL);
	luaL_dofile(gL, "login.lua");

	//must after lua_dofile(gL, "login.lua")
	db_init();//use lua to access mysql

	conn_server* s = conn_server_new();//for all the client
	s->port = cfg->login_client_port;
	s->conn_cb = _login_client_conn;
	s->read_cb = _login_client_read;
	s->error_cb = _login_client_error;
	int ret = conn_server_startup(s);
	if (ret < 0) {
		log_error();
		exit(0);
	}

	conn_server* s2 = conn_server_new();//for all the client
	s2->port = cfg->login_realm_port;
	s2->conn_cb = _login_realm_reg;
	s2->read_cb = _login_realm_data;
	s2->error_cb = _login_realm_unreg;
	ret = conn_server_startup(s2);
	if (ret < 0) {
		log_error();
		exit(0);
	}

	return 0;
}

static session*
_conn_get_session(long uid)
{
	session* s = (session*)calloc(1, sizeof(session));
	assert(s);
	lua_getglobal(gL, "stbl");
	lua_pushinteger(gL, uid);
	lua_gettable(gL, -2);
	lua_tostring(gL, -1);
}

//-------------------------------------------------

static void
_login_client_conn(conn_client* c)
{}

//-------------------------------------
//没有用google protocol buffer
//-------------------------------------
static void
_login_client_read(conn_client* c)
{
	buffer* buf = c->rbuf;
	char* ptr = buf->ptr;
	size_t offset = buf->offset;
	size_t size = buf->size;
	size_t used = buf->used;
	if (offset >= used)//没有数据读取
		return;
	size_t total_len = used - offset;
	//1个byte的chunk len
	BYTE chunk_len = (BYTE)(*(ptr+offset));
	if (total_len - LEN_BYTE < chunk_len) {//没有收到一个完整的chunk
		log_msg();
		return;
	}
	offset += LEN_BYTE;//chunk len size
	//2个byte的proto id
	WORD proto_id = (WORD)(*(WORD*)(ptr+offset));

	offset += LEN_WORD;

	BYTE urs_len;
	switch (proto_id) {
	case S_CHECK:
		urs_len = (BYTE)(*(ptr + offset));//1个byte的string的长度
		offset += LEN_BYTE;
		char* urs = ptr + offset;
		offset += urs_len;

		BYTE pwd_len = (BYTE)(*(ptr + offset));//1个byte的string的长度
		offset += LEN_BYTE;
		char* pwd = ptr + offset;
		offset += pwd_len; 

		buf->offset = offset;//移动rbuf->offset

		//push argument to lua
		lua_pushlstring(gL, urs, urs_len);
		lua_pushlstring(gL, pwd, pwd_len);
		lua_getglobal(gL, "on_check_user");
		int code = lua_pcall(gL, 2, 1, 0);
		if (code != 0) {
			lua_pop(gL, 1);
			return;
		}

		//pop return value from lua
		LWORD uid = lua_tointeger(gL, -1);
		lua_pop(gL, 1);
		const char* skey = lua_tostring(gL, -1);
		BYTE slen = strlen(skey);

		char buf[1024];
		size_t package_len = LEN_HEADER + LEN_LWORD + 1 + slen;
		DWORD p_id = 2;
		memcpy(buf, (void*)&package_len, sizeof(BYTE));
		memcpy(buf + sizeof(BYTE), (void*)&p_id, sizeof(DWORD));
		memcpy(buf + sizeof(BYTE) + sizeof(DWORD), (void*)&uid, sizeof(LWORD));
		memcpy(buf + sizeof(BYTE) + sizeof(DWORD) + sizeof(LWORD), (void*)&slen, 1);

		memcpy(buf + sizeof(BYTE) + sizeof(DWORD) + sizeof(LWORD) + 1, (void*)skey, slen);

		buffer_append(c->wbuf, buf, package_len);
		conn_client_send(c);
		break;
	default:
		//-------------------------
		//to do: kick out the user, destroy client_conn
		//-------------------------
		conn_kick_client(c);
		break;
	}
}

//------------------------------------
//用google protocol buffer
//------------------------------------
static void
_login_client_read2(conn_client* c)
{
	buffer* buf = c->rbuf;
	char* ptr = buf->ptr;
	size_t offset = buf->offset;
	size_t size = buf->size;
	size_t used = buf->used;
	if (offset >= used)//没有数据读取
		return;

	//-----------------------------
	//to do.....
	//-----------------------------

	//解析msg头，找出msg id和msg len
	int msg_id = 0;
	int msg_len = 0;

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

//----------------------------------------
static void
_login_client_error(conn_client* c)
{}

static void
_login_realm_reg(conn_client* c)
{}

static void
_login_realm_data(conn_client* c)
{
	/*
	int proto_id;
	switch (proto_id) {
	case CONFIRM:
		int uid = 1341234;
		session* s = _conn_get_session(uid);
		char buf[1024];
		pack(buf, s);
		conn_client_send(c, buf);

		//
		//to do: send session info to realm
		//

		session_free(s);//must free session
		break;
	default:
		break;
	}
	*/
}

static void
_login_realm_unreg(conn_client* c)
{}

static void conn_client_send(conn_client* c)
{
	buffer* buf = c->wbuf;
	if (buf->used - buf->offset <= 0)
		return;

	bufferevent_write(c->bev, buf->ptr, buf->used-buf->offset);
}
