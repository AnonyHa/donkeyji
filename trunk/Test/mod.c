extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <dirent.h>//GNU c library
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class huji
{
public:
	int func(int a) {
		a++;
		printf("a = %d\n", a);
		return a;
	}
};

class wrapper_huji : public huji
{
public:
	int func(lua_State* L) {
		int a = luaL_checkinteger(L, -1);
		return huji::func(a);
	}
};

static int new_huji(lua_State* L)
{
	huji* ph = (huji*)lua_newuserdata(L, sizeof(huji));
	luaL_getmetatable(L, "huji");
	lua_setmetatable(L, -2);
	return 1;
}

//huji:func(1)
static int func(lua_State* L)
{
	wrapper_huji* wh = (wrapper_huji*)luaL_checkudata(L, 1, "huji");
	return wh->func(L);
}

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

static int _f(lua_State* L)
{
	luaL_error(L, "in _f");
	return 0;
}

static int new_t(lua_State* L)
{
	//int top = lua_gettop(L);
	lua_newtable(L);
	lua_pushstring(L, "one");
	lua_pushinteger(L, 5);
	lua_settable(L, -3);
	printf("--------\n");
	return 1;
}

static int obj_len(lua_State* L)
{
	int len = lua_objlen(L, -1);
	printf("len = %d\n", len);
	int i;

	for (i=1; i<=len; i++) {
		lua_rawgeti(L, -1, i);
		printf("val = %d\n", lua_tointeger(L, -1));
		printf("type = %s\n", lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}

	return 0;
}

static int travel(lua_State* L)
{
	int top = lua_gettop(L);

	if (!lua_istable(L, -1)) {
		luaL_typerror(L, -1, "table");
		printf("*******\n");
		return 0;
	}

	lua_pushnil(L);
	int r[2];
	int i = 0;

	while (lua_next(L, -2) != 0) {
		printf("%s---%s\n",
		       lua_typename(L, lua_type(L, -2)),
		       lua_typename(L, lua_type(L, -1))
		      );
		printf("key = %s\n", lua_tostring(L, -2));
		//lua_pop(L, 1);
		lua_pushvalue(L, -2);
		r[i] = luaL_ref(L, LUA_REGISTRYINDEX);
		i++;
		lua_pop(L, 1);
	}

	for (i=0; i<2; i++) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, r[i]);
		printf("---%s\n", lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}

	lua_settop(L, top);
	return 0;
}

static int new_env(lua_State* L)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "f");

	if (!lua_isfunction(L, -1)) {
		lua_settop(L, top);
		luaL_error(L, "not function");
	}

	lua_newtable(L);
	int ret = lua_setfenv(L, -2);
	printf("------ret = %d\n", ret);
	lua_pcall(L, 0, 0, 0);
	return 0;
}

// ---------------------------------------
static const struct luaL_Reg hujilib[] = {
	{"new_huji", new_huji},
	{NULL, NULL}
};


// -------------
// 面向对象访问
// -------------
static const struct luaL_Reg method[] = {
	{"func", func},
	{NULL, NULL}
};

int luaopen_hujilib(lua_State* L)
{
	printf("----luaopen_hujilib----\n");
	luaL_newmetatable(L, "huji");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_register(L, NULL, method);
	luaL_register(L, "hujilib", hujilib);
	return 1;
}
