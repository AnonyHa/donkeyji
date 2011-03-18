#include "ltimer.h"
#include "timer.h"

lua_State* gL = NULL;

int l_timer_init()
{
	gL = luaL_newstate();
	luaL_openlibs(gL);

	luaopen_timer(gL);
	return 0;
}

void l_multi_cb(void* arg)
{
	int ref = (int)arg;
	lua_getref(gL, ref);

	if (!lua_isfunction(gL, -1)) {
		return;
	}

	if (lua_pcall(gL, 0, 0, 0)) {
		//倘若调用脚本出错，不处理，直接返回即可，不退出进程
		printf("lua_pcall failed\n");
		return;
	}

	//lua_call(gL, 0, 0);
}

void l_once_cb(void* arg)
{
	int ref = (int)arg;
	lua_getref(gL, ref);

	if (!lua_isfunction(gL, -1)) {
		return;
	}

	if (lua_pcall(gL, 0, 0, 0)) {
		return;
	}
}

static int l_multi_call(lua_State* L)
{
	if (!lua_isfunction(L, 1)) {
		luaL_error(L, "not a function");
		return 0;
	}

	double timeout = (double)luaL_checknumber(L, 2);
	lua_pop(L, 1);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);//为function创建一个ref，保存为将来使用
	multi_call(timeout, l_multi_cb, (void*)ref);
}

static int l_once_call(lua_State* L)
{
	if (!lua_isfunction(L, 1)) {
		luaL_error(L, "not a function");
		return 0;
	}

	double timeout = (double)luaL_checknumber(L, 2);
	lua_pop(L, 1);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);//为function创建一个ref，保存为将来使用
	once_call(timeout, l_once_cb, (void*)ref);
}

static const struct luaL_reg timers[] = {
	{"multi_call", l_multi_call},
	{"once_call", l_once_call},
	{NULL, NULL},
};

int luaopen_timer(lua_State* L)
{
	luaL_register(L, "timer", timers);
}
