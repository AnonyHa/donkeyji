#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

const char* DO_STAT = "do_stat.lua";

extern int luaopen_lposix(lua_State* L);

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_lposix(L);
	int error = luaL_loadfile(L, DO_STAT) || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error){
		printf("[error]\n");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	return 0;
}

