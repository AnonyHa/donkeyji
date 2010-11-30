//3.CMysql类的实现
#include "afxdb.h"

int mysql_db_init(lua_State* L, const char* db, const char* user, const char* pwd, const char* host, unsigned int port)
{
	lua_getglobal(L, "mysql_db_init");
	lua_pushstring(L, db);
	lua_pushstring(L, user);
	lua_pushstring(L, pwd);
	lua_pushstring(L, host);
	lua_pushinteger(L, port);
	int ret = lua_pcall(L, 5, 0, NULL);
	return ret;
}
