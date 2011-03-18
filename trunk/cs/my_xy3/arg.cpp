#include "arg.h"
#include <iostream>

// -------------------------------
// data_in: luaջ��index
// buf: pack���ŵ�λ��
// buf_len: buf�Ŀ��ó���
// ����ֵ: pack�β������ĵ�buf����
// -------------------------------
int ArgDword::pack(lua_State* L, int data_in, byte* buf, int buf_len)
{
	int used_len = 4;
	int num;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));

	if (used_len > buf_len) {
		return -1;    // ��ʾ����
	}

	memcpy(buf, &num, sizeof(num));
	return  used_len;
}

// --------------------------------------
// buf: ��Ҫunpack�����ݵ�λ��
// buf_len: buf��ʣ����Ҫ��������ݵĳ���
// ����ֵ: unpack�˲�������buf�ĳ���
// --------------------------------------
int ArgDword::unpack(lua_State* L, const byte* buf, int buf_len)
{
	int readed_len = 4;

	if (readed_len > buf_len) {
		return -1;    // ����
	}

	lua_pushnumber(L, *(const int*)buf);//ѹ��lua��ջ
	return readed_len;
}


// ------------------------------------------------------------
// ����string�ĳ��Ȳ��ǹ̶��ģ�������Ҫ����string�ĳ�����Ϊ��ͷ
// ------------------------------------------------------------
int ArgString::pack(lua_State* L, int data_in, byte* buf, int buf_len)
{
	int used_len = 0;
	size_t str_len;
	const char* data = luaL_checklstring(L, data_in, &str_len);
	int str_id = static_cast<int>(str_len) + 1;

	if (str_id <= 0xffff) {// ���֧�ֵ��ַ���
		used_len = static_cast<int>(str_len+2);// ��ͷ2���ֽ���װstring�ĳ���

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
	lua_pushlstring(L, (const char*)(buf+2), str_len);// ѹ��luaջ
	readed_len += str_len;
	return readed_len;
}

// **********************************************************
// **********************************************************

// --------------------
// double�ͣ���number��
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
std::vector<ArgBase*> ArgMgr::_args;// static ��Ա��ʼ��
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
