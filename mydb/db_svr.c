#include "afxdb.h"
#include "stdlib.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <string>
#include <lua5.1/lua.hpp>

#include "protocol.h"

//---------------------------
//鍏ㄥ眬鏁版嵁
int _listen_sock;
int _listen_port;
int _state;

struct game_server* g_gs = NULL;
struct db_cfg* g_conf = NULL;
proto_manager* g_pm = NULL;
lua_State* g_L = NULL;

struct db_buf* g_in_buf = NULL;
struct db_buf* g_out_buf = NULL;
//---------------------------


//-------------------------
static const char* DB_SVR_ADDR = "127.0.0.1";
std::string cur_char = "server";

int hook_send (const unsigned char* buf,  int len, int vfd, int ismulticast = 0, int mcpayloadlen = 0)
{}


int FlushOutSockBuf ( )
{
	int ret = 0 ;
	int Size = 0 ;

	if ((Size = db_buf_size(g_out_buf)) <= 0) {
		return 0;
	}

	// 触发epoll的WRITE事件
	ret = bufferevent_write(g_gs->_bev, db_buf_head(g_out_buf), Size);

	if (ret == 0) {
		db_buf_seek_head(g_out_buf, Size) ;
		//printf("engine send to netd size:%d\n", Size) ;
	}

	if (errno != EAGAIN && ret < 0) {//缓冲区装不下
		int InputLen = 0, OutputLen = 0;
		InputLen = EVBUFFER_LENGTH(g_gs->_bev -> input);
		OutputLen = EVBUFFER_LENGTH(g_gs->_bev -> output);
		//_ERROR("write socket error=%d,strerror=%s, inlen = %d, outlen = %d", errno, strerror(errno), InputLen, OutputLen);
	}

	return ret ;
}

#define MAX_INTERNAL_CONN 2

static void db_free()
{
	if (g_gs != NULL) {
		free(g_gs);
	}

	if (g_conf != NULL) {
		free(g_conf);
	}

	if (g_L != NULL) {
		lua_close(g_L);
	}

	if (g_pm != NULL) {
		delete(g_pm);
	}
}

static void db_new()
{
	//init pointer
	g_gs = NULL;
	g_conf = NULL;

	g_gs = gs_new();

	if (g_gs == NULL) {
		goto clean;
	}

	g_conf = conf_new();

	if (g_conf == NULL) {
		goto clean;
	}

	g_L = luaL_newstate();

	if (g_L == NULL) {
		goto clean;
	}

	// use c++ implementation
	g_pm = new proto_manager(hook_send);

clean:
	db_free();
}

static void socket_setnonblocking(int socket_fd)
{
	int flags = fcntl(socket_fd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(socket_fd, F_SETFL, flags);
}

static int conn_game()
{
	struct db_cfg* cfg = g_conf;
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_fd < 0) {
		exit(-1);
	}

	int optval = 1;

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		exit(-1);
	}

	struct sockaddr_in addr;

	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;

	inet_aton(DB_SVR_ADDR, &addr.sin_addr);

	addr.sin_port = htons((u_short)cfg->_listen_port);

	if (bind(listen_fd, (struct sockaddr * ) &addr, sizeof(addr))<0) {
		exit(-1);
	}

	if (listen(listen_fd, MAX_INTERNAL_CONN) < 0) {
		exit(-1);
	}

	struct sockaddr_in client_addr;

	socklen_t addr_len = sizeof(client_addr);

	int sock_fd = -1;

	sock_fd = accept(listen_fd, (sockaddr *)&client_addr, &addr_len);

	if ( sock_fd < 0 ) {
		exit(-1);
	}

	socket_setnonblocking(sock_fd);

	return sock_fd;
}

//callback
static void gs_input(struct bufferevent *bufev, void *arg)
{}

//callback
static void gs_error(struct bufferevent *bufev, short what, void *arg)
{}

static int net_init()
{
	struct game_server* gs = g_gs;
	assert(gs != NULL);
	gs->_fd = conn_game();
	//gs->_state =
	gs->_bev = bufferevent_new(gs->_fd, gs_input, NULL, gs_error, NULL);
}

static int register_all_signals()
{}

static int register_all_timers()
{}

int db_init()
{
	int ret;
	struct db_cfg* cfg;
	struct Database_Param* dp;

	//read config file
	ret = conf_init(g_conf);

	if (ret < 0) {
		goto clean;
	}

	//libevent init
	log_init();
	//proto_init();
	event_init();
	register_all_signals();
	register_all_timers();

	script_init(g_L, g_conf->_script_file);

	//connect to mysql
	cfg = g_conf;
	ret = mysql_db_init(
	          g_L,
	          cfg->_mysql_db,
	          cfg->_mysql_user,
	          cfg->_mysql_pwd,
	          cfg->_mysql_addr,
	          cfg->_mysql_port
	      );

	if (ret) {}

	//begin to listen until get game server connection fd
	ret = net_init();

	if (ret < 0) {
		goto clean;
	}

clean:
	db_free();
	return -1;
}

int db_run()
{
	event_dispatch();
}
