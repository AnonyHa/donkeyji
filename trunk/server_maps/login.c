#include "conn.h"
#include "login.h"

//------------------------------------------
//static function declaration
static void _login_conn_cb(conn_client* c);
static void _login_read_cb(conn_client* c);
static void _login_error_cb(conn_client* c);
//------------------------------------------
lua_State* gL = NULL;

int 
login_init()
{
	//lua init
	gL = lua_newstate();
	lua_openlibs(gL);
	luaopen_luasql_mysql(gL);
	lua_dofile(gL, "login.lua");

	//must after lua_dofile(gL, "login.lua")
	db_init();//use lua to access mysql

	conn_server* s = conn_server_new();
	s->port = cfg->port;
	s->conn_cb = _login_conn_cb;
	s->conn_read_cb = _login_read_cb;
	s->conn_error_cb = _login_error_cb;
	int ret = conn_server_startup(s);
	if (ret < 0) {
		log_error();
		exit(0);
	}
	return 0;
}

static void 
_login_conn_cb(conn_client* c)
{}

static void 
_login_read_cb(conn_client* c)
{}

static void 
_login_error_cb(conn_client* c)
{}

int add_session(long uid, buffer* skey)
{
	lua_pushinteger(L, uid);
	lua_pushlstring(L, skey->ptr, skey->used);
	lua_getglobal(L, "add_session");
	lua_pcall(L, 0, 0, NULL);
}

session* get_session(long uid)
{
	session* s = (session*)calloc(1, sizeof(session));
	assert(s);
	lua_getglobal(L, "stbl");
	lua_pushinteger(L, uid);
	lua_gettable(L, -2);
	lua_tostring(L, -1);
}
