#include "conn.h"
#include "login.h"

//------------------------------------------
//static function declaration
static void _login_conn_cb(conn_client* c);
static void _login_read_cb(conn_client* c);
static void _login_error_cb(conn_client* c);
//------------------------------------------

int login_init() 
{
	conn_server* s = conn_server_new();
	s->port = cfg->port;
	s->conn_cb = _login_conn_cb;
	s->conn_read_cb = _login_read_cb;
	s->conn_error_cb = _login_error_cb;
	int ret = conn_server_startup(s);
	if (ret < 0) {
		return -1;
	}
	return 0;
}

static void _login_conn_cb(conn_client* c)
{}

static void _login_read_cb(conn_client* c)
{}

static void _login_error_cb(conn_client* c)
{}
