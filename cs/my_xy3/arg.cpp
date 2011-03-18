#include "arg.h"
#include <iostream>

// -------------------------------
// data_in: lua栈的index
// buf: pack后存放的位置
// buf_len: buf的可用长度
// 返回值: pack次参数消耗的buf长度
// -------------------------------
int ArgDword::pack(lua_State* L, int data_in, byte* buf, int buf_len)
{
	int used_len = 4;
	int num;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));

	if (used_len > buf_len) {
		return -1;    // 表示出错
	}

	memcpy(buf, &num, sizeof(num));
	return  used_len;
}

// --------------------------------------
// buf: 需要unpack的数据的位置
// buf_len: buf还剩的需要解包的数据的长度
// 返回值: unpack此参数消耗buf的长度
// --------------------------------------
int ArgDword::unpack(lua_State* L, const byte* buf, int buf_len)
{
	int readed_len = 4;

	if (readed_len > buf_len) {
		return -1;    // 出错
	}

	lua_pushnumber(L, *(const int*)buf);//压到lua中栈
	return readed_len;
}


// ------------------------------------------------------------
// 由于string的长度不是固定的，所以需要加上string的长度作为包头
// ------------------------------------------------------------
int ArgString::pack(lua_State* L, int data_in, byte* buf, int buf_len)
{
	int used_len = 0;
	size_t str_len;
	const char* data = luaL_checklstring(L, data_in, &str_len);
	int str_id = static_cast<int>(str_len) + 1;

	if (str_id <= 0xffff) {// 最大支持的字符串
		used_len = static_cast<int>(str_len+2);// 用头2个字节来装string的长度

		if (used_len > buf_len) {
			return -1;
		}

		memcpy(buf, &str_id, 2);// string length
		memcpy(buf+1, data, str_len);
	} else {
		return -1;
	}
}

int ArgString::unpack(lua_State* L, const byte* buf, int buf_len)
{
	int readed_len = 2;

	if (readed_len > buf_len) {
		return -1;
	}

	size_t str_len;
	memcpy(&str_len, buf, 2);
	lua_pushlstring(L, (const char*)(buf+2), str_len);// 压入lua栈
	readed_len += str_len;
	return readed_len;
}

// **********************************************************
// **********************************************************

// --------------------
// double型，即number型
// --------------------
int ArgNumber::pack(lua_State* L, int data_in, byte* buf, int buf_len)
{
	int used_len = 0;
	double data = luaL_checknumber(L, data_in);

	if (data == 0) {
		return -1;
	}

	size_t data_len = sizeof(data);
	used_len += data_len;

	if (used_len > buf_len) {
		return -1;
	}

	memcpy(buf, &data, used_len);
	return used_len;
}

int ArgNumber::unpack(lua_State* L, const byte* buf, int buf_len)
{
	int readed_len = sizeof(double);
	double data;

	if (readed_len > buf_len) {
		return -1;
	}

	lua_pushnumber(L, *((const double*)buf));
	return readed_len;
}

// -------------
// ArgMgr
// -------------
std::vector<ArgBase*> ArgMgr::_args;// static 成员初始化
ArgMgr* ArgMgr::_argMgr = NULL;

ArgMgr* ArgMgr::instance()
{
	if (_argMgr == NULL) {
		_argMgr = new ArgMgr();
	}

	return _argMgr;
}

void ArgMgr::init()
{
	_args.push_back(new ArgString());
	_args.push_back(new ArgDword());
	_args.push_back(new ArgNumber());
}

void ArgMgr::destruct()
{
	for (int i=0; i<_args.size(); i++) {
		if (_args[i] != NULL) {
			delete _args[i];
		}
	}
}

ArgBase* ArgMgr::getArg(const char* type_name)
{
	for (int i=0; i<_args.size(); i++) {
		if (_args[i]->checkType(type_name)) {
			return _args[i];
		}
	}

	return NULL;
}
