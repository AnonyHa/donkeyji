#include <stdio.h>
#include <string.h>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define PRELOAD "./script/base/preload.lua"

int luaopen_hujilib(lua_State*);
//-----------------------------------------
int traceback(lua_State* L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");

	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}

	lua_getfield(L, -1, "excepthook");

	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}

	lua_pushvalue(L, 1);
	lua_pcall(L, 1, 1, 0);

	printf("=========\n");

	return 1;
}

int mypcall(lua_State* L, int nargs, int nrets, int errfunc)
{
	int base = lua_gettop(L) - nargs;
	lua_pushcfunction(L, traceback);
	lua_insert(L, base);
	int status = lua_pcall(L, nargs, nrets, base);
	lua_remove(L, base);
	return status;
}


int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_hujilib(L);
	int error = luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0);

	if (error) { //此时lua会把错误信息string放在栈顶
		printf("[error]");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
		lua_pop(L, 1);
		return -1;
	}

	lua_getglobal(L, "huge");
	lua_pushinteger(L, 33);
	error = mypcall(L, 1, 0, 0);
	if (error) {
		printf("[error]");
		return -1;
	}

	lua_close(L);

	return 0;
}
