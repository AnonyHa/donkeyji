//extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
//}
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


static int test(lua_State* L)
{
	const char* p = luaL_checkstring(L, 1);
	printf("%s\n", p);
	return 0;
}

// ---------------------------------------
static const struct luaL_Reg hujilib[] = {
	{"test", test},
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
	luaL_newmetatable(L, "huji");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_register(L, NULL, method);
	luaL_register(L, "hujilib", hujilib);
	return 1;
}
