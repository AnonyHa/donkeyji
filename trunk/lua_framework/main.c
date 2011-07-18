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
int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_hujilib(L);
	int error = luaL_loadfile(L, PRELOAD) || lua_pcall(L, 0, LUA_MULTRET, 0);

	if (error) { //��ʱlua��Ѵ�����Ϣstring����ջ��
		printf("[error]");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//ȡջ���Ĵ�����Ϣ
		lua_pop(L, 1);
	}

	lua_close(L);

	return 0;
}
