#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <dirent.h>//GNU c library
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void _t(lua_State* L)
{
	int top = lua_gettop(L);
	lua_pushinteger(L, 1);
	printf("after push: top = %d\n", lua_gettop(L));
	lua_settop(L, top);
	printf("after set: top = %d\n", lua_gettop(L));
}

static int test(lua_State* L)
{
	//lua_istable(L, -1);
	int top = lua_gettop(L);
	printf("top = %d\n", top);
	_t(L);
	printf("out: top = %d\n", lua_gettop(L));
	return 0;
}

// ---------------------------------------
static const struct luaL_Reg hujilib[] = {
	{"test", test},
	{NULL, NULL}
};


// -------------
// 面向对象访问
// -------------
static const struct luaL_Reg method[] = {
	{NULL, NULL}
};

static const struct luaL_Reg method2[] = {
	{NULL, NULL}
};


int luaopen_hujilib(lua_State* L)
{
	//luaL_newmetatable(L, "LuaBook.dir");
	// 为原表设置__gc字段
	//lua_pushstring(L, "__gc");
	//lua_pushcfunction(L, dir_gc); 
	//lua_settable(L, -3);
	luaL_register(L, "hujilib", hujilib);
	return 1;
}
