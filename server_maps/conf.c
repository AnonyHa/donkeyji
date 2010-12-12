#include <sm.h>

#include "conf.h"

//global config
config* cfg = NULL;

typedef struct _kv
{
	const char* name;
	const char* type;
}kv;

kv items[] = {
	{"server_mode", "number"},
	{"log_level", "number"},
	{"log_file", "string"},
	{"login_port", "number"},
	{"login_max_conn", "number"},
	{NULL, NULL},//as the end
};

#define c2c(name) cfg->name

void 
conf_init() 
{
	cfg = (config*)calloc(1, sizeof(config));
	cfg->log_file = (char*)calloc(1, 20);
	cfg->log_dir = (char*)calloc(1, 20);
	cfg->doc_root = (char*)calloc(1, 20);

	//set default config
	cfg->login_port = 10031;
	cfg->login_max_conns = 20000;

	cfg->log_level = LOG_DEBUG;
	cfg->log_file = "sm.log";
	cfg->is_daemon = 0;

	//use lua as config file
	lua_State* L = luaL_newstate();	
	luaL_openlibs(L);
	int error = luaL_loadfile(L, "sm.conf") || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (error != 0) {
		printf("load conf file failed\n");
		fprintf(stderr, "%s\n", lua_tostring(L, -1));//取栈顶的错误信息
		lua_pop(L, 1);
		exit(1);
	}
	
	int i;
	for (i=0; items[i].name!=NULL &&items[i].type!=NULL; i++) {
		const char* name = items[i].name;
		const char* type = items[i].type;
		lua_getfield(L, -1, name);
		if (strcmp(type, "string") == 0) {
			if (!lua_isstring(L, -1)) {
				exit(1);
			}
			const char* tmp = lua_tostring(L, -1);
			memcpy(c2c(name), (void*)tmp, strlen(tmp));
			lua_pop(L, 1);
		} else if (strcmp(type, "number") == 0) {
			if (!lua_isnumber(L, -1)) {
				exit(0);
			}
			c2c(name) = lua_tonumber(L, -1);
			lua_pop(L, 1);
		} else {
			printf("wrong type: %s\n", type);
			exit(1);
		}
	}

	lua_close(L);

	conf_print();
}

int 
conf_destroy()
{}

void 
conf_print()
{
	/*
	printf("port : %d\n", cfg->port);
	printf("max_conns : %d\n", cfg->max_conns);
	printf("log_level : %d\n", cfg->log_level);
	printf("log_fle : %s\n", cfg->log_file);
	printf("log_dir : %s\n", cfg->log_dir);
	printf("doc_root : %s\n", cfg->doc_root);
	printf("daemon : %d\n", cfg->daemon);
	*/
}

void 
conf_free()
{
}
