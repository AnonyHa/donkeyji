#include <stdio.h>
#include "lua.h" 
#include "lualib.h"
#include "lauxlib.h"

#define PRELOAD "preload.lua"

int traceback (lua_State *L)
{
	printf("-----\n");
	fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
	/*
	printf("-------in traceback------\n");
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_getfield(L, -1, "traceback");
	lua_pcall(L, 0, 0, 0);
	*/
	return 1;
}

void infinite_loop(lua_State* L, lua_Debug* ar)
{
	printf("----loop----\n");
}

int reset_eval_cost(lua_State *L)
{
	//lua_sethook(L, infinite_loop, LUA_MASKCOUNT, 10);
	//lua_sethook(L, infinite_loop, LUA_MASKCALL, 0);
	return 0;
}
/*
int docall (lua_State *L, int narg, int nresults, int)
{
	int status;
	reset_eval_cost(L);
	int base = lua_gettop(L) - narg;  //function index
	lua_pushcfunction(L, traceback);  //push traceback function
	lua_insert(L, base);  //put it under chunk and args
	status = lua_pcall(L, narg, nresults, base);//base是error handle function，即traceback函数
	lua_remove(L, base);  //remove traceback function
	return status;
}
*/

//-----------------------------------------
int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_hujilib(L);
	int error = luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error){//此时lua会把错误信息string放在栈顶
		printf("[error]");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
		lua_pop(L, 1);
	}

	lua_getglobal(L, "f");
	lua_call(L, 0, 0);
	int ret;
	//int ret = docall(L, 0, 0, 0);
	if (ret != 0) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
	}
	
	lua_close(L);

	return 0;
}
