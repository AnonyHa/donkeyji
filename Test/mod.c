#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <dirent.h>//GNU c library
#include <errno.h>
#include <string.h>
#include <stdlib.h>


static int test(lua_State* L)
{
	lua_istable(L, -1);
	return 0;
}

// ---------------------------------------
static const struct luaL_Reg hujilib[] = {
	{"test", test},
	{NULL, NULL}
};

// -------------
// ����������
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
	// Ϊԭ������__gc�ֶ�
	//lua_pushstring(L, "__gc");
	//lua_pushcfunction(L, dir_gc); 
	//lua_settable(L, -3);
	luaL_register(L, "hujilib", hujilib);
	return 1;
}
