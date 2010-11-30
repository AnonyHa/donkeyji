#include <stdio.h>
#include "lua.hpp"//lua.hpp包含了lua.h lualib.h lauxlib.h
#include "windows.h"
#include "test.h"

#define PRELOAD "preload.lua"

int luaopen_hujilib(lua_State*);	
extern int _unpack_ref;

//调用lua函数的接口
void call_va(lua_State* L, const char* func, const char* sig, ...)
{
	va_list vl;
	int narg;
	int nres;

	va_start(vl, sig);

	//函数入栈
	lua_getfield(L, LUA_GLOBALSINDEX, func);

	//参数入栈
	for (narg=0; *sig; narg++)
	{
		luaL_checkstack(L, 1, "too many argument");
		switch (*sig++)
		{
		case 'd':
			lua_pushnumber(L, va_arg(vl, double));
			break;
		case 'i':
			lua_pushinteger(L, va_arg(vl, int));
			break;
		case 's':
			lua_pushstring(L, va_arg(vl, char*));
			break;
		case '>':
			goto endargs;
			break;
		default:
			break;
		}
	}
endargs:
	nres = strlen(sig);
	printf("nres = %d\n", nres);

	//完成调用
	if (lua_pcall(L, narg, nres, 0) != 0)
		printf("error\n");

	//结果检测
	nres = -nres;	
	while (*sig) {
		switch(*sig++) {
		case 'd':
			if (!lua_isnumber(L, nres))
				printf("wrong result");
			*va_arg(vl, double*) = lua_tonumber(L, nres);
			break;
		case 'i':
			*va_arg(vl, int*) = lua_tointeger(L, nres); 
			break;
		case 's':
			*va_arg(vl, const char**) = lua_tostring(L, nres);
			break;
		default:
			break;
		}
		nres++; 
	}

	va_end(vl);
}
int traceback (lua_State *L)
{
	printf("------00000000\n");
	printf("-------in traceback------\n");
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		printf("1111\n");
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "excepthook");
	if (!lua_isfunction(L, -1)) {
		printf("2222\n");
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  //pass error message
	lua_pcall(L, 1, 1, 0);  //call debug.traceback
	printf("3333\n");
	return 1;
}

int ResetEvalCost(lua_State *L)
{
	return 0;
}
int docall (lua_State *L, int narg, int nresults, int)
{
	int status;
	ResetEvalCost(L);
	int base = lua_gettop(L) - narg;  //function index
	lua_pushcfunction(L, traceback);  //push traceback function
	lua_insert(L, base);  //put it under chunk and args
	status = lua_pcall(L, narg, nresults, base);//base是error handle function，即traceback函数
	lua_remove(L, base);  //remove traceback function
	return status;
}

//-----------------------------------------
int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);//调用luaL_register注册内部的模块，table, io, string, math, debug等等
	luaopen_hujilib(L);//注册扩展模块
	//test.lua的全局变量a保存在lua_State中，接下来调用user.lua时，该全局变量a仍然存在，可以直接引用
	int error = luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error){//此时lua会把错误信息string放在栈顶
		printf("[error]");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
		lua_pop(L, 1);
	}
	//luaL_error(L, "outside");

	/*
	error = luaL_loadfile(L, "mod.lua") || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error){
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	lua_State* L1 = lua_newthread(L);
	lua_getglobal(L1, "f");
	lua_pushinteger(L1, 5);
	int ret = lua_resume(L1, 1);
	if (ret == LUA_YIELD) {
		printf("%d\n", lua_gettop(L1));
		printf("%d\n", lua_tointeger(L1, 1));
		printf("%d\n", lua_tointeger(L1, 2));
	}
	ret = lua_resume(L1, 0);
	if (ret == 0) {
		printf("%d\n", lua_gettop(L1));
		printf("%d\n", lua_tointeger(L1, 1));
	}
	*/

	/*
	//lua_getglobal(L, "tbl");
	lua_getfield(L, LUA_GLOBALSINDEX, "tbl");
	if (!lua_istable(L, -1))
		lua_error(L);
	lua_getfield(L, -1, "one");
	int a = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "two");
	int b = lua_tointeger(L, -1);
	lua_pop(L, 1);

	printf("one = %d, two = %d\n", a, b);
	*/

	/*
	lua_getfield(L, LUA_GLOBALSINDEX, "f");
	lua_pushnumber(L, a);
	lua_pushnumber(L, b);
	lua_pcall(L, 2, 2, 0);
	int r1 = lua_tointeger(L, -2);
	int r2 = lua_tointeger(L, -1);
	printf("r1 = %d, r2 = %d\n", r1, r2);
	*/

	//call_va(L, "f", "ii>ii", a, b);

	/*
	lua_getglobal(L, "f");
	int ret = docall(L, 0, 0, 0);
	if (ret != 0) {
	}
	*/
	
	lua_close(L);

	return 0;
}
