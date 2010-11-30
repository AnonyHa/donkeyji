#ifndef GLOBAL_H
#define GLOBAL_H

#include "afxdb.h"
//#include <lua5.1/luasql.h>

int mysql_db_init(struct lua_State* L, const char* db, const char* user, const char* pwd, const char* host, unsigned int port);

#endif
