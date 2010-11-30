#include <string.h>
#include "server.h"
#include <lua5.1/lua.hpp>
#include <sys/time.h>
#include "proto_s.h"

#define PRELOAD "./preload_s.lua"

// -------------
// 全局的网络模块
// -------------
Server ss(1300);

// ---------------------
// 启动lua，注册扩展库
// ---------------------
lua_State* L = NULL;
void start_lua()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	open_net_libs(L);
}

// ------------
// 全局单例变量
// ------------
ProtoMgr_s* pto_mgr = ProtoMgr_s::instance();

// --------------
// 全局的发送函数
// --------------
void hook_send_s(int vfd, byte* data, int len)
{
	ss.doSend(vfd, data, len);
}

// ----------------------
// on_***均为事件处理函数
// ----------------------
int on_new(Event* e)
{
	return 0;
}

int on_data(Event* e)
{
	pto_mgr->unpackData(L, (const byte*)e->_data, e->_len, e->_hid);
	return 0;
}

int on_timer(Event* e)
{
	return 0;
}

int on_leave(Event* e)
{
	return 0;
}

// ---------------
int predo()
{
	start_lua();

	int err =( luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0) );
	if (err)
		return -1;

	ss.startUp();
	return 0;
}

int postdo()
{
	delete pto_mgr;
	lua_close(L);
	return 0;
}


int main_loop()
{
	Event* e;
	while (1) {
		//ss.process();
		ss.dispatch();
		e = ss.doRead();
		if (e != NULL) {//完整的协议数据包
			switch (e->_event)
			{
			case NET_NEW:
				on_new(e);
				break;
			case NET_DATA:
				on_data(e);
				break;
			case NET_TIMER:
				on_timer(e);
				break;
			case NET_LEAVE:
				on_leave(e);
				break;
			}
			delete e;// 消耗掉该event, e始终位于_queue中
		}
		lua_gc(L, LUA_GCSTEP, 0);
		//usleep(500000);
		//usleep(1000000);
	}
	return 0;
}

int main()
{
	int ret = predo();
	if (ret == -1)
		return -1;

	main_loop();

	postdo();// 不会执行到这里

	return 0;
}
