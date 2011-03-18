#ifndef _ARG_H
#define _ARG_H


#include <string>
#include <vector>
#include <lua5.1/lua.hpp>

typedef unsigned char byte;

// --------------------------------------------------
// ֻ������ʾ�������ͣ��Լ��ṩpack��unpack�ķ�����
// �����ṩ���ݴ洢������pack��unpackʱҪ�ṩ������
// ---------------------------------------------------
class ArgBase
{
protected:
	std::string _type;
public:
	ArgBase() {
		_type = "base";
	}
	virtual ~ArgBase() {};
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len) = 0;
	virtual int unpack(lua_State* L, const byte* buf, int buf_len) = 0;
	const char* getType() {
		return _type.c_str();
	}
	bool checkType(const char* type_name) {
		return _type == type_name;
	}
};

class ArgDword : public ArgBase
{
public:
	ArgDword() {
		_type = "dword";
	}
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State* L, const byte* buf, int buf_len);
};

class ArgNumber : public ArgBase
{
public:
	ArgNumber() {
		_type = "number";
	}
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State* L, const byte* buf, int buf_len);
};

class ArgString : public ArgBase
{
public:
	ArgString() {
		_type = "string";
	}
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State* L, const byte* buf, int buf_len);
};

class ArgArray : public ArgBase
{
public:
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len) {}
	virtual int unpack(lua_State* L, const byte* buf, int buf_len) {}
};

class arg_table : public ArgBase
{
public:
	virtual int pack(lua_State* L, int data_in, byte* buf, int buf_len) {}
	virtual int unpack(lua_State* L, const byte* buf, int buf_len) {}
};


// ------------------------------------
// ȫ��Ψһ��һ��ʵ����ȫ����static��Ա
// ------------------------------------
class ArgMgr
{
private:
	static std::vector<ArgBase*> _args;
	static ArgMgr* _argMgr;
	ArgMgr() {
		init();
	}
public:
	~ArgMgr() {
		destruct();
	}

public:
	static ArgMgr* instance();
	void init();
	void destruct();
	void addArg(ArgBase*);
	void addTableArgs(lua_State* L);
	ArgBase* getArg(int arg_id);
	ArgBase* getArg(const char* type_name);
};

#endif
