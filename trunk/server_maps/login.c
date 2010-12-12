#include "conf.h"
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
	/*
	int proto_id;

	switch (proto_id) {
	case S_CHECK:
		buffer* urs = NULL;
		buffer* pwd = NULL;
		lua_pushlstring(gL, urs->ptr, urs->used);
		lua_pushlstring(gL, pwd->ptr, pwd->used);
		lua_getglobal(gL, "on_check_user");
		lua_pcall(gL, 2, 1, 0);
		//-------------------------
		//get return value from lua
		//-------------------------
		int uid = get_uid();
		buffer* skey = get_skey();
		char buf[1024];
		pack(buf, uid, skey);
		conn_client_send(c, buf);
		break;
	default:
		//-------------------------
		//to do: kick out the user, destroy client_conn
		//-------------------------
		conn_client_del(c);
		break;
	}
	*/
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
