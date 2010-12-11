#include "conf.h"
#include "login.h"

//extern lua_State* gL;

int db_init()
{
	lua_pushlstring(gL, cfg->db_name);
	lua_pushlstring(gL, cfg->db_name);
	lua_pushlstring(gL, cfg->db_pwd);
	lua_pushlstring(gL, cfg->db_host);
	lua_pushlstring(gL, cfg->db_port);
	lua_getglobal(L, "db_init");
	lua_pcall(gL, 4, 0, NULL);
	return 0;
}
