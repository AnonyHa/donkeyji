#ifndef _LOGIN_H
#define _LOGIN_H

typedef struct _session
{
	long uid;
	buffer* skey;//session key
}session;

//hash table, save session
typedef struct _session_tbl
{}session_tbl;


//---------------------------
int login_init();

extern lua_State* gL;
#endif
