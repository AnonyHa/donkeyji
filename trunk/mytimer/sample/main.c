#include "timer.h"
#include "ltimer.h"

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void cb2(void* arg)
{
	printf("cb2: %s\n", arg);
}

void cb1(void* arg)
{
	printf("cb1: %s\n", arg);
}

void cb3(void* arg)
{
	printf("cb3: %s\n", arg);
}

int main()
{
	//-----------------
	// init each module
	//-----------------
	timer_init();
	l_timer_init();

	if (luaL_loadfile(gL, "test.lua") || lua_pcall(gL, 0, LUA_MULTRET, 0)) {
		printf("do test error\n");
		fprintf(stderr, "%s\n", lua_tostring(gL, -1));
		lua_pop(gL, 1);
		//这里初始化脚本出错，就立即退出
		return -1;
	}

	//-------------
	//logic
	//-------------
	//multi_call(1, cb1, "--------hello");
	//once_call(3, cb1, "fuck---");


	timer_loop();

	//--------------------
	// destroy each module
	//--------------------
	timer_destory();

	return 0;
}
