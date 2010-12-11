#include "conn.h"
#include "login.h"

//------------------------------------------
//static function declaration
static void _login_conn_cb(conn_client* c);
static void _login_read_cb(conn_client* c);
static void _login_error_cb(conn_client* c);
//------------------------------------------
struct session_tbl* stbl = NULL;//uid为key，存储session
struct base_info* bif = NULL;//存储各个realm的基本信息


int 
login_init() 
{
	stbl = session_tbl_new();
	if (stbl == NULL) {
		log_error();
		exit(0);
	}
	bif = base_info_new();
	if (stbl == NULL) {
		log_error();
		exit(0);
	}

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
