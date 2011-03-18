#include "client.h"
#include <lua5.1/lua.hpp>
#include <vector>
#include <string>
#include "proto_c.h"

// ---------------------------------
#define PRELOAD "./preload_c.lua"
#define LOGIC "./logic_c.lua"
// ---------------------------------

Client cc;

// ---------------------
// 启动lua，注册扩展库
// ---------------------
lua_State* L = NULL;
void start_lua()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	open_net_libs(L);

	int err =( luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0) );
}

ProtoMgr_c* pto_mgr = ProtoMgr_c::instance();

void hook_send_c(byte* data, int len)
{
	std::cout<<"----hook_send_c----"<<std::endl;
	cc.doSend(data, len);
}

void on_data(char* data, int len)
{
	cout<<"---on_data"<<endl;
	unsigned char pto_id = *((unsigned char*)(data));
	int id = (int)pto_id;
	cout<<"pto_id recv = "<<id<<endl;
	pto_mgr->unpackData(L, (const byte*)data, len);
}

int predo()
{
	start_lua();
	cc.doConnect("127.0.0.1", 1300);
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
	while (1) {
		cc.process();
		// 发送id = 1的协议包
		//luaL_dofile(L, LOGIC);
		short size;
		char* p = cc.doRecv(size);

		if (p != NULL && size != 0) {
			on_data(p, size);
			delete p;
		}

		lua_gc(L, LUA_GCSTEP, 0);
		sleep(1);
	}
}

int main()
{
	predo();
	main_loop();
	postdo();
	return 0;
}
