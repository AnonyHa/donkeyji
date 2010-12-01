#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"


//global config
config* cfg = NULL;

int conf_init() 
{
	cfg = (config*)calloc(1, sizeof(config));
	cfg->log_file = (char*)calloc(1, 20);
	cfg->log_dir = (char*)calloc(1, 20);
	cfg->doc_root = (char*)calloc(1, 20);

	//use lua as config file
	lua_State* L = luaL_newstate();	
	luaL_openlibs(L);
	int error = luaL_loadfile(L, "mp.conf") || lua_pcall(L, 0, LUA_MULTRET, 0);
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

	lua_getfield(L, -1, "log_dir");
	tmp = lua_tostring(L, -1);
	len = strlen(tmp);
	memcpy((void*)cfg->log_dir, (const void*)tmp, len);
	lua_pop(L, 1);//pop "log_dir"
	lua_pop(L, 1);//pop "log"

	lua_getfield(L, -1, "doc_root");
	tmp = lua_tostring(L, -1);
	len = strlen(tmp);
	memcpy((void*)cfg->doc_root, (const void*)tmp, len);
	lua_pop(L, 1);//pop "log_dir"

	lua_close(L);

	return 0;
}

void conf_print()
{
	printf("port : %d\n", cfg->port);
	printf("max_fds : %d\n", cfg->max_fds);
	printf("log_level : %d\n", cfg->log_level);
	printf("log_fle : %s\n", cfg->log_file);
	printf("log_dir : %s\n", cfg->log_dir);
	printf("doc_root : %s\n", cfg->doc_root);
}

int conf_free()
{
	if (cfg == NULL)
		return 0;
	if (cfg->log_file != NULL)
		free(cfg->log_file);
	if (cfg->log_dir != NULL)
		free(cfg->log_dir);
	if (cfg->doc_root != NULL)
		free(cfg->doc_root);
	free(cfg);
	return 0;
}
