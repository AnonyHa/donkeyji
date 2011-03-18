#include "afxdb.h"

int script_init(lua_State* L, const char* script_file)
{
	int ret = luaL_loadfile(L, script_file) || lua_pcall(L, 0, LUA_MULTRET, 0);

	if (ret != 0) {
		exit(-1);
	}

	return ret;
}
