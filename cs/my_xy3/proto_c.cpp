#include "proto_c.h"
#include <assert.h>
#include <iostream>

extern lua_State* L;
extern void hook_send_c(byte* data, int len);

// --------------------------------
ProtoMgr_c* ProtoMgr_c::_pto_mgr = NULL;
std::vector<Pto_c*> ProtoMgr_c::_ptos;

ProtoMgr_c::ProtoMgr_c()
{
	pto_cnt = 0;
	// �����ǵ�һ�ε���ArgMgr::instance()
	ArgMgr* arg_mgr = ArgMgr::instance();
}

ProtoMgr_c::~ProtoMgr_c()
{
	for (int i=0; i<_ptos.size(); i++) {
		delete _ptos[i];
	}

	ArgMgr* arg_mgr = ArgMgr::instance();
	delete arg_mgr;
}

ProtoMgr_c* ProtoMgr_c::instance()
{
	if (_pto_mgr == NULL) {
		_pto_mgr = new ProtoMgr_c();
	}

	return _pto_mgr;
}

Pto_c* ProtoMgr_c::getPto(int pto_id)
{
	return _ptos[pto_id];
}

int ProtoMgr_c::unpackData(lua_State* L, const byte* buf, int buf_size)
{
	int readed_len = 1;

	if (readed_len > buf_size) {
		return -1;
	}

	unsigned int proto_id = 0;
	memcpy(&proto_id, buf, 1);

	/*
	if (proto_id == 0) {
		proto_id = 0;
		readed_len += 2;
		if (readed_len > buf_size)
			return -1;
		memcpy(&proto_id, buf+1, 2);
	}
	*/
	if (proto_id >= _ptos.size()) {
		return -1;
	}

	std::cout<<"proto_id = "<<proto_id<<std::endl;
	Pto_c* pto = _ptos[proto_id];

	if (pto) {
		// buf_size-readed_len: ��ȥ��proto_id��İ���
		int cur_readed = pto->unpack(L, buf+readed_len, buf_size-readed_len);

		if (cur_readed < 0) {
			return -1;
		}

		if (cur_readed == 0 && pto->_args.size() > 0) {
			return -1;
		}

		readed_len += cur_readed;
		return readed_len;
	} else {
		return -1;
	}
}

// ----------------------------------------------
Pto_c::Pto_c(int id)
{
	_id = id;
	_ref = LUA_NOREF;
	_isMaker = false;
}

int Pto_c::load(lua_State* L)// L������luaֱ�Ӵ������ģ�����C����������
{
	int top = lua_gettop(L);//����top��index�����ļ���

	if (procType(L, luaL_checkstring(L, -1)) == -1) {
		return -1;
	}

	std::string file = luaL_checkstring(L, -1);
	int err = luaL_loadfile(L, file.c_str()) || lua_pcall(L, 0, 1, 0);//ֱ��ִ��.pto�ļ�

	if (err == 0) {
		return convertPto(L);//��ʱջ��ΪЭ��������table
	} else {
		lua_settop(L, top);//�ָ�ջ��
		return -1;//������һ��ĺ����������
	}
}

int Pto_c::convertPto(lua_State* L)
{
	int top = lua_gettop(L);

	if (!lua_istable(L, -1)) {
		return -1;    //������ֱ�ӵ�lua��չ������һ�����������
	}

	int ret = procFunc(L);

	if (ret == -1) {
		lua_settop(L, top);//�ָ�ջ��
		return -1;
	}

	ret = procArg(L);

	if (ret == -1) {
		lua_settop(L, top);//�ָ�ջ��
		return -1;
	}

	lua_settop(L, top);//�ָ�ջ��
	return 0;//��ʾ�ɹ�
}

int Pto_c::procFunc(lua_State* L)
{
	int top = lua_gettop(L);//��ӱ�lua���õ�c��������֤ÿ�ε��ý�����ջ�����䣬����lua��һ���ջ��

	if (_isMaker) {
		lua_pushstring(L, "for_maker");
		lua_rawget(L, LUA_GLOBALSINDEX);

		if (!lua_istable(L, -1)) {
			lua_pushstring(L, "[proto error]: can't find maker's funcs!");
			lua_error(L);
			return -1;
		}

		lua_pushstring(L, _func.c_str());
		lua_rawget(L, -2);//��ʱջ��Ϊfunction

		if (!lua_isfunction(L, -1)) {
			lua_pushstring(L, "[proto error]: can't find far_func!");
			lua_error(L);
			return -1;
		}

		if (_ref != LUA_NOREF) {
			lua_unref(L, _ref);
		}

		_ref = luaL_ref(L, LUA_REGISTRYINDEX);//�����������LUA_REGISTRYINDEX
		assert(_ref);

		lua_settop(L, top);//�ָ�ջ��
		return 0;
	} else {
		lua_pushstring(L, "for_caller");
		lua_rawget(L, LUA_GLOBALSINDEX);//ջ��Ϊfor_caller

		if (!lua_istable(L, -1)) {
			//std::cout<<"no for_caller"<<std::endl;
			lua_newtable(L);
			lua_setglobal(L, "for_caller");// �൱��for_caller = {}
			lua_pushstring(L, "for_caller");
			lua_rawget(L, LUA_GLOBALSINDEX);
			assert(lua_istable(L, -1));
		}

		lua_pushstring(L, _func.c_str());//ջ��֮��Ϊ_func.c_str()
		lua_pushlightuserdata(L, this);
		lua_pushcclosure(L, pack, 1);//ջ��Ϊclosure
		lua_rawset(L, -3);//for_caller[_func.c_str()] = closure

		lua_settop(L, top);//�ָ�ջ��
		return 0;
	}

	lua_settop(L, top);//�ָ�ջ��
	return -1;
}

int Pto_c::procArg(lua_State* L)
{
	int top = lua_gettop(L);
	lua_pushstring(L, "arg_list");
	lua_rawget(L, -2);

	if (lua_istable(L, -1)) {// ��ʱջ��Ϊarg_list table
		int arg_list_index = lua_gettop(L);

		for (int i=1; i<=static_cast<int>(lua_objlen(L, arg_list_index)); i++) {
			lua_settop(L, arg_list_index);//����Ҫ
			lua_rawgeti(L, arg_list_index, i);

			if (!lua_istable(L, -1)) {
				lua_pushstring(L, "[proto error]: invalid arg_list");
				lua_error(L);
				return -1;
			}

			int desc_index = lua_gettop(L);
			lua_rawgeti(L, desc_index, 2);//�Ȳ������������ͣ�û�е�3������
			std::string arg_type = luaL_checkstring(L, -1);
			ArgBase* arg = NULL;
			arg = (ArgMgr::instance())->getArg(arg_type.c_str());// ͨ�����������ȡ

			if (arg) {
				_args.push_back(arg);
			} else {
				lua_pushstring(L, "[proto error]: read invalid arg_list");
				lua_error(L);
				lua_settop(L, top);//�ָ�ջ��
				return -1;
			}
		}

		lua_settop(L, top);//�ָ�ջ��
		return 0;
	}

	lua_settop(L, top);//�ָ�ջ��
	return -1;
}

// -------
// c����
// -------
int Pto_c::unpack(lua_State* L, const byte* buf, int buf_len)
{
	std::cout<<"buf_len = "<<buf_len<<std::endl;

	int top = lua_gettop(L);//����ΪʲôҪ����ջ����???
	int readed_len = 0;

	if (_ref == LUA_NOREF) {
		std::cout<<"no unpack function"<<std::endl;
		return -1;
	}

	lua_getref(L, _ref);
	assert(lua_isfunction(L, -1));// ???

	for (size_t i=0; i<_args.size(); i++) {
		assert(_args[i]);
		int cur_readed = _args[i]->unpack(L, buf+readed_len, buf_len-readed_len);

		if (cur_readed == 0) {
			std::cout<<"unpack argument fail"<<std::endl;
			lua_settop(L, top);
			return -1;
		}

		readed_len += cur_readed;
	}

	int status = lua_pcall(L, _args.size(), 0, 0);

	if (status) {
		std::cout<<"run lua function error"<<std::endl;
		lua_settop(L, top);
		return -1;
	} else {
		lua_settop(L, top);
		return readed_len;
	}
}

int Pto_c::marshal(lua_State* L, byte* buf, size_t buf_len)
{
	int used_len = 0;
	memcpy(buf, &_id, 1);// _id���
	used_len += 1;

	for (size_t i=0; i<(int)_args.size(); i++) {
		// ��i+2�𣬼���index = 2����Ϊvfd�Ѿ���pack�д������
		int cur_used = _args[i]->pack(L, i+1, buf+used_len, buf_len-used_len);

		if (cur_used == 0) {
			lua_pushstring(L, "pack argument fail");
			lua_error(L);
			return -1;
		}

		used_len += cur_used;
	}

	return used_len;
}

// -----------
// luaֱ�ӵ���
// -----------
int Pto_c::pack(lua_State* L)
{
	int top = lua_gettop(L);// ����ջ��
	Pto_c* self = static_cast<Pto_c*>(lua_touserdata(L, lua_upvalueindex(1)));

	byte buf[100];
	int used_len = self->marshal(L, buf, 100);

	if (used_len) {
		hook_send_c(buf, used_len);
	}

	lua_settop(L, top);// �ָ�ջ��
	return 0;
}

int Pto_c::procType(lua_State* L, const char* file_name)
{
	_func = file_name;

	if (_func[0] == 'c') {
		_isMaker = 1;
		return 0;
	} else if (_func[0] == 's') {
		_isMaker = 0;
		return 0;
	} else {
		return -1;
	}
}

int ProtoMgr_c::addPto(lua_State* L)
{
	unsigned int pto_id = _ptos.size();
	std::cout<<"pto_id = "<<pto_id<<std::endl;
	Pto_c* p = new Pto_c(pto_id);
	int ret = p->load(L);//proto_s::load

	if (ret == 0) {
		_ptos.push_back(p);
		lua_pushnumber(L, pto_id);
		return 1;
	} else {
		lua_pushstring(L, "load pto failed!");
		lua_error(L);// ���˳�������ִ��return���
		return 1;
	}
}

// --------------
// ��չ��lua�ӿ�
// --------------
static const luaL_Reg networklib[] = {
	{"add_pto", ProtoMgr_c::addPto},
	{NULL, NULL}
};

// ------------
// �����ⲿ����
// ------------
int open_net_libs(lua_State* L)
{
	luaL_register(L, "networklib", networklib);
}
