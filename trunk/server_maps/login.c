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

static session* _conn_get_session(long uid);
//------------------------------------------
lua_State* gL = NULL;

int
login_init()
{
	//lua init
	gL = luaL_newstate();
	luaL_openlibs(gL);
	//luaopen_luasql_mysql(gL);
	luaL_dofile(gL, "login.lua");

	//must after lua_dofile(gL, "login.lua")
	db_init();//use lua to access mysql

	conn_server* s = conn_server_new();//for all the client
	s->port = cfg->port;
	s->conn_cb = _login_client_conn;
	s->read_cb = _login_client_read;
	s->error_cb = _login_client_error;
	int ret = conn_server_startup(s);
	if (ret < 0) {
		log_error();
		exit(0);
	}

	conn_server* s2 = conn_server_new();//for all the client
	s2->port = cfg->port;
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
	SHORT proto_id = (SHORT)(*(SHORT*)(ptr+offset));

	offset += LEN_SHORT;

	switch (proto_id) {
	case S_CHECK:
		BYTE urs_len = (BYTE)(*(ptr + offset));//1个byte的string的长度
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
		long uid = lua_tointeger(gL, -1);
		int uid = get_uid();
		lua_pop(gL, 1);
		const char* skey = lua_tostring(gL, -1);
		size_t slen = strlen(skey);

		char buf[1024];
		unsigned char package_len = LEN_HEADER + sizeof(long) + 1 + slen;
		unsigned int p_id = 2;
		memcpy(buf, (void*)&package_len, sizeof(unsigned char));
		memcpy(buf + sizeof(unsigned char), (void*)&p_id, sizeof(unsigned int));
		memcpy(buf + sizeof(unsigned char) + sizeof(unsigned int), (void*)&uid, sizeof(long));
		memcpy(buf + sizeof(unsigned char) + sizeof(unsigned int) + sizeof(long), (void*)&slen, 1);

		memcpy(buf + sizeof(unsigned char) + sizeof(unsigned int) + sizeof(long) + 1, (void*)skey, slen);

		buffer_append(c->wbuf, buf, package_len);
		conn_client_send(c);
		break;
	default:
		//-------------------------
		//to do: kick out the user, destroy client_conn
		//-------------------------
		conn_client_del(c);
		break;
	}
}

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
