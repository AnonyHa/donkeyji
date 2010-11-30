#ifndef _PROTO_S_H
#define _PROTO_S_H

#include <string>
#include <vector>
#include <lua5.1/lua.hpp>
#include "arg.h"

class ProtoMgr_s;
class Pto_s
{
	friend class ProtoMgr_s;
private:
	unsigned char _id;
	std::string _func;
	std::vector<ArgBase*> _args;//只保存指针，对象存储在ArgMgr中
	int _isMaker;
	int _ref;
public:
	Pto_s(int id);
	int load(lua_State* L);
	int procType(lua_State* L, const char* file_name);
	int procFunc(lua_State* L);
	int procArg(lua_State* L); 
	int convertPto(lua_State* L);
	int unpack(lua_State* L, const byte* buf, int buf_len, int hid);
	static int pack(lua_State* L);
	int marshal(lua_State* L, byte* buf, size_t buf_len);
};

class ProtoMgr_s
{
private:
	static std::vector<Pto_s*> _ptos;
	int pto_cnt;
	static ProtoMgr_s* _pto_mgr;
private:
	ProtoMgr_s();
public:
	~ProtoMgr_s();
public:
	static ProtoMgr_s* instance();
	static int addPto(lua_State* L);
	Pto_s* getPto(int pto_id);
	int unpackData(lua_State* L, const byte* buf, int len, int vfd);
};

// -------------------------------
// 单例对象的内存需要在外部delete
// -------------------------------
int open_net_libs(lua_State* L);
#endif
