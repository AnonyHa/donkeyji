#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <string.h>

#include "conf.h"


//global config
config* cfg;

int conf_init() 
{
	cfg = (config*)calloc(1, sizeof(config));
	cfg->log_file = (char*)calloc(1, 20);
	cfg->log_dir = (char*)calloc(1, 20);
	cfg->doc_root = (char*)calloc(1, 20);

	//use lua as config file
	lua_State* L = luaL_newstate();	
	luaL_openlibs(L);
	int error = luaL_loadfile(L, "conf.lua") || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error != 0) {
		printf("load conf file failed\n");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
		lua_pop(L, 1);
		return -1;
	}
	
	lua_getglobal(L, "server");
	if (!lua_istable(L, -1)) {
		printf("server should be a table\n");
		return -1;
	}
	lua_getfield(L, -1, "network");

	if (!lua_istable(L, -1)) {
		printf("network should be a table\n");
		return -1;
	}
	lua_getfield(L, -1, "port");
	if (!lua_isnumber(L, -1)) {
		printf("port should be a table\n");
		return -1;
	}
	cfg->port = lua_tointeger(L, -1);
	lua_pop(L, 1);//pop "port"

	lua_getfield(L, -1, "max_fds");
	if (!lua_isnumber(L, -1)) {
		printf("port should be a table\n");
		return -1;
	}
	cfg->max_fds = lua_tointeger(L, -1);
	lua_pop(L, 1);//pop "max_fds"
	lua_pop(L, 1);//pop "network"

	lua_getfield(L, -1, "log");
	if (!lua_istable(L, -1)) {
		printf("log should be a table\n");
		return -1;
	}
	lua_getfield(L, -1, "log_level");
	cfg->log_level = lua_tointeger(L, -1);
	lua_pop(L, 1);//pop "log_level"

	lua_getfield(L, -1, "log_file");
	const char* tmp = lua_tostring(L, -1);
	int len = strlen(tmp);
	memcpy((void*)cfg->log_file, (const void*)tmp, len);
	lua_pop(L, 1);//pop "log_file"

	lua_getfiled(L, -1, "log_dir");
	const char* tmp = lua_tostring(L, -1);
	int len = strlen(tmp);
	memcpy((void*)cfg->log_dir, (const void*)tmp, len);
	lua_pop(L, 1);//pop "log_dir"
	lua_pop(L, 1);//pop "log"

	lua_getfield(L, -1, "doc_root");
	const char* tmp = lua_tostring(L, -1);
	int len = strlen(tmp);
	memcpy((void*)cfg->doc_root, (const void*)tmp, len);
	lua_pop(L, 1);//pop "log_dir"

	lua_close(L);

	return 0;
}
