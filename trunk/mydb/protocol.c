// $Id: protocol.cpp 67112 2008-12-24 07:35:05Z tony $

#include "protocol.h"
#include <string>

extern struct db_buf* g_out_buf;
extern int FlushOutSockBuf ( );
#define VFD int
#define VFD_NULL 200000

//////////////////////////////////////////////////////////////////////////
// hash�ַ���������Э��У��
static int calc_hashnr(const char *key, size_t length)
{
	int nr=1, nr2=4;

	while (length--) {
		nr^= (((nr & 63)+nr2)*((int) (char) *key++))+ (nr << 8);
		nr2+=3;
	}

	return nr;
}

//////////////////////////////////////////////////////////////////////////
static const  std::string client = "client";
static const  std::string server = "server";
// ��������Э��������Ϣ�еĹؼ���
static const char func_name[] = "func_name";
static const char arg_list[] = "arg_list";
static const  char for_caller[] = "for_caller";
static const  char for_maker[] = "for_maker";

size_t net_protocol::_total_pack_count = 0 ;
size_t net_protocol::_total_unpack_count = 0 ;

net_protocol::net_protocol(int id):_args(), _pack_func_name()
{
	_id = id;
	_is_maker = false;
	_unpack_ref = LUA_NOREF;
	_pack_count = 0 ;
	_unpack_count = 0 ;
}

net_protocol::~net_protocol()
{
	_args.clear();
}

int net_protocol::load(lua_State *L)
{
	// ջ��ΪЭ�������ļ�·��
	int top = lua_gettop(L);

	// �����ļ�������Э������
	if (process_type(L, luaL_checkstring(L, -1)) == PROTO_ERROR) {
		return PROTO_ERROR;
	}

	// ����Э������
	const int buf_len = 100 * 1024;
	char read_buf[buf_len];
	const char * file = luaL_checkstring(L, -1) ;
	int len = read_file(file, read_buf, buf_len);

	if (len== 0) {
		raise_error(L, "[proto error]: fail to read proto_desc file=%s\n!", file);
		return PROTO_ERROR;
	}

	int status = luaL_loadbuffer(L, read_buf, len, 0);

	if (0 == status && lua_isfunction(L, -1)) {
		lua_newtable(L); //�ÿռ��ػ�����ȷ����ȫ����

		if (lua_setfenv(L, -2) != 1) {
			lua_settop(L, top);
			return PROTO_ERROR;
		}

		int status = lua_pcall(L, 0, 1, 0);

		if (status) {
			lua_settop(L, top);
			return PROTO_ERROR;
		}

		return convert_protocol(L);
	} else {
		raise_error(L, "[proto error]: invalid proto_desc file=%s!\n", file);
		//lua_settop(L, top);
		//����ƽ��ջ�����ǰѳ�����ַ������ظ��ϲ㣬
		//����ջй¶����Ϊһ������������˳��ˡ�
		lua_settop(L, top + 1);
		return PROTO_ERROR;
	}
}

int net_protocol::process_type(lua_State *L, const char* path)
{
	std::string name = path;
	size_t begin_pos = name.find_last_of('/');

	if (begin_pos < 0) {
		begin_pos = name.find_last_of('\\');
	}

	size_t end_pos = name.find_last_of('.');

	if ((begin_pos < 0) || (end_pos < 0) || (begin_pos >= end_pos)) {
		raise_error (L, "[proto error]: invalid protocol path,\n<%s>!\n", path);
		return PROTO_ERROR;
	}

	++begin_pos; // ȥ��"/"
	_pack_func_name = name.substr(begin_pos, end_pos-begin_pos);

	if (_pack_func_name.substr(0, 2) == "c_") {
		_is_maker = (cur_char == client);
	} else if (_pack_func_name.substr(0, 2) == "s_") {
		_is_maker = (cur_char == server);
	} else {
		raise_error (L, "[proto error]: invalid protocol path! \n<%s>!\n", path);
		return PROTO_ERROR;
	}

	return PROTO_OK;
}

int net_protocol::convert_protocol(lua_State*L)
{
	int top = lua_gettop(L);

	// ջ������ΪЭ���ļ����غ󷵻ص�table
	if (!lua_istable(L, -1)) {
		return PROTO_ERROR;
	}

	// ������������
	lua_pushstring(L, func_name);
	lua_rawget(L, -2);

	if (lua_isstring(L, -1)) {
		_pack_func_name = lua_tostring(L, -1);
	}

	lua_pop(L, 1);

	// ����Э������
	int rel = func_proc(L);

	if (rel != PROTO_OK) {
		lua_settop(L, top);
		return rel;
	}

	// ���������б�
	rel = arglist_proc(L);

	if (rel != PROTO_OK) {
		lua_settop(L, top);
		return rel;
	}

	lua_settop(L, top);
	return PROTO_OK;
}

int net_protocol::func_proc(lua_State* L)
{
	int top = lua_gettop(L);

	if (_is_maker) {
		// ��ǰΪԶ�̺���ʵ����
		lua_pushstring(L, for_maker);
		lua_rawget(L, LUA_GLOBALSINDEX);

		if (!lua_istable(L, -1)) { // ���ڴ���Ҫʵ�ֵ�Զ�̺������������ն�Զ�̵���
			raise_error(L, "[proto error]: can't find maker's funcs!");
			return PROTO_ERROR;
		}

		lua_pushstring(L, _pack_func_name.c_str());
		lua_rawget(L, -2);

		if (!lua_isfunction(L, -1)) {
			raise_error(L, "[proto error]:can't find far_func:%s", _pack_func_name.c_str());
			return PROTO_ERROR;
		}

		if (_unpack_ref != LUA_NOREF) {
			lua_unref(L, _unpack_ref);
		}

		_unpack_ref = lua_ref(L, true);
		assert(_unpack_ref != LUA_NOREF);
		lua_settop(L, top);
		return (_unpack_ref != LUA_NOREF) ? PROTO_OK : PROTO_ERROR;
	} else {
		// ��ǰΪԶ�̺���������
		lua_pushstring(L, for_caller);
		lua_rawget(L, LUA_GLOBALSINDEX);

		if (!lua_istable(L, -1)) {
			// ���ڴ�Ź�Զ�̵��õĺ���
			lua_newtable(L);
			lua_setglobal(L, for_caller);
			lua_pushstring(L, for_caller);
			lua_rawget(L, LUA_GLOBALSINDEX);
			assert(lua_istable(L, -1));
		}

		// ��upvalue��Э���ʽ���ݰ󶨵��������
		lua_pushstring(L, _pack_func_name.c_str());
		lua_pushlightuserdata(L, this);
		lua_pushcclosure(L, pack, 1);
		lua_rawset(L, -3);
		lua_settop(L, top);
		return PROTO_OK;
	}

	return PROTO_ERROR;
}

int net_protocol::arglist_proc(lua_State* L)
{
	// ջ��Ϊ��¼Э���ʽ��table
	lua_pushstring(L, arg_list);
	lua_rawget(L, -2);

	if (lua_istable(L, -1)) {
		int arg_list_i = lua_gettop(L);

		for (int i=1; i<=static_cast<int>(lua_objlen(L, arg_list_i)); ++i) {
			lua_settop(L, arg_list_i);
			lua_rawgeti(L,arg_list_i, i);		// ��ȡ����������Ϣ

			if (!lua_istable(L, -1)) {
				raise_error(L, "[proto error]: invalid arg_list desc!");
				return PROTO_ERROR;
			}

			int desc_i = lua_gettop(L);
			lua_rawgeti(L, desc_i, 2);	// ��ȡ��������
			std::string arg_type = luaL_checkstring(L, -1);
			lua_rawgeti(L, desc_i, 3);
			fcall_base_arg* arg = 0;

			if (lua_isnumber(L, -1)) {	// �Ƿ�Ϊ����
				// �����Ͳ���
				int array_len = static_cast<int>(lua_tonumber(L, -1));

				if (array_len < 0 || array_len > 255) {
					raise_error(L, "[proto error]: array error:%s!", arg_type.c_str());
					return PROTO_ERROR;
				}

				arg = new fcall_array_arg(arg_type.c_str(), array_len);
				assert(arg);
				fcall_arg_manager::add_arg(arg);
			} else {
				arg = fcall_arg_manager::get_arg(arg_type.c_str());
			}

			if (arg) {
				_args.push_back(arg);
			} else {
				raise_error(L, "[proto error]: read invalid arg_list!");
				return PROTO_ERROR;
			}
		}

		return PROTO_OK;
	}

	return PROTO_ERROR;
}

int net_protocol::marshal(lua_State * L, byte * buf, size_t buf_len)
{
	int used_len = 0 ;
	// ������Э��id
	assert(this->_id != 0); // ���0���ڱ��Э��ų���

	if (this->_id < 256) {
		memcpy(buf, &this->_id, 1);
		used_len = 1 ;
	} else {
		*buf = 0;
		memcpy(buf+1, &this->_id, 2);
		used_len = 3 ;
	}

	// �������
	for (int i=0; i<(int)this->_args.size(); ++i) {
		assert(this->_args[i]);
		// i + 2 ��ʾlua stack ��Ĳ�����λ��
		int cur_used = this->_args[i]->pack(L, i+2, buf+used_len, buf_len-used_len);

		if (cur_used == 0) {
			raise_error (L,"pack argument fail. protocol_id:%d, nth_arg:%d, expect_type:%s\n",
			             this->get_id(), i, this->_args[i]->get_type());
			return -1;
		}

		used_len += cur_used;
	}

	return used_len ;
}
// ��ע�ᵽ��������ʱ���Ϊ��̬����
// ��Ϊ���ڶ����ڽű���ֱ�Ӵ������pack ������error���ű��� , �ű���handle ���error.
// ���ڽű��� for_caller.func(vfd, ...) ��ʱ�򣬾͵��õ����������
// ��raise_error�����Ļ��������������cpu���ع���, �������ܻ�Ӿ����ز�����ѩ����
// ��Ϊ���������Ӻ͵��Է��㣬������raise_error����_RUNTIME_ERROR�ɡ�
int net_protocol::pack(lua_State* L)
{
	//ÿ��pack������ FlushOutSockBuf
	FlushOutSockBuf();

	int top = lua_gettop(L);
	net_protocol *self = static_cast<net_protocol*>(lua_touserdata(L, lua_upvalueindex(1)));

	//�����Ƿ���ȷpack������ͳ��
	net_protocol::_total_pack_count++ ;
	self->_pack_count++ ;

	//��ȡ���Ӳ���(����������Զ�̺����ĵ�һ������)
	const int type = lua_type (L, 1) ;

	if (type == LUA_TNUMBER) {
		VFD ext = static_cast<int>(lua_tonumber(L, 1));

		if  (VFD_NULL >= ext) {
			//_WARN("drop unicast to VFD_NULL ptoid=%d,VFD_NULL=%d", self->get_id(), VFD_NULL) ;
			goto outway ;
		}

		// ��ȡ����
		byte * buf = (byte*)db_buf_alloc_pack(g_out_buf, MAXTONETD);

		if (buf == NULL) {
			//print_runtime_error (L,"oom to pack the unicast protocol ptoid=%d\n", self->get_id()) ;
			goto outway ;
		}

		int used_len = self->marshal (L, buf, MAXTONETD) ;

		if (used_len > 0) {
			proto_manager::s_data_sender(buf, used_len, ext, 0, 0);
		} else {
			//print_runtime_error (L,"fail to marshal unicast ptoid=%d,usedsize=%d\n", self->get_id(), used_len) ;
			printf("\n");
		}
	} else if (type == LUA_TTABLE) {
		//byte data[MAXTONETD] ;Ϊ�˼���һ��memcpy������ֱ�Ӵ�g_out_buf��ֱ�ӷ���һ�����ڴ�
		const size_t s = lua_objlen (L, 1) ;

		if (s > 0) {
			//�����������ݰ����ڴ�
			byte * data = (byte*)db_buf_alloc_pack(g_out_buf, MAXTONETD) ;

			if (data == NULL) {
				//print_runtime_error (L,"oom to pack the multicast protocol ptoid=%d,size=%d\n", self->get_id(), MAXTONETD) ;
				goto outway ;
			}

			int used_len = self->marshal (L, data, MAXTONETD) ;

			if (used_len > 0) {
				size_t c = 0 , t = 0 ;
				byte * beginp = &data[used_len] ;
				byte * endp = &data[MAXTONETD] ;

				for (size_t i = 0 ; i < s ; ++i) {
					lua_pushnumber (L, i + 1) ;
					lua_gettable(L, 1) ;
					const VFD to = luaL_checkinteger (L, -1) ;
					lua_remove(L, -1) ;

					if (to > VFD_NULL) {
						t++ ;

						if (beginp+sizeof(VFD) <= endp) {
							*(VFD*)beginp = to ;
							beginp += sizeof(VFD) ;
							c++ ;
						}
					}
				}

				if (c > 0) {
					size_t total = used_len + c * sizeof(VFD) ;
					proto_manager::s_data_sender(data, total, c, 1, used_len);

					if (c < t) { //ֻ�㲥һ����vfd.�����Ѿ������ˣ�Ȼ���raise��֪ͨ��Ϣ
						//print_runtime_error (L,"partly multicast because of oom ptoid=%d,\
						datasize=%d, multivfd=%d,realvfd=%d\n", self->get_id(), total, t, c ) ;
						                                 printf("\n");
					                                 }
					                                 } else {//c == 0
						                                 if ( t == 0 )
						                                 printf("\n");
						                                 //_WARN("drop multicast to VFD_NULL ptoid=%d", self->get_id()) ;
						                                         else  // t > 0 ˵������û���κοռ�洢Ŀ�ĵ�vfd
						                                         //print_runtime_error (L,"drop total multicast because of oom ptoid=%d,\ datasize=%d,multivfdsize=%d,realvfdsize=%d\n", self->get_id(),used_len, t, c ) ;
						                                                 printf("\n");
					                                                 }
					                                                 } else { //used_len
						                                                 //print_runtime_error (L,"fail to marshal multicast ptoid=%d,usedsize=%d\n", self->get_id(), used_len) ;
						                                                         printf("\n");
					                                                         }
					                                                         } // lua_objlen
					                                                         } else { //type ==

						                                                         //print_runtime_error (L,"unknow destination type for protocol id=%d, type=%d\n", self->get_id(), type) ;
						                                                                 printf("\n");
					                                                                 }


						                                                                 outway :
						                                                                 lua_settop(L, top);
						                                                                 return 0;
					                                                                 }

						                                                                 // Ҫ�����ǽ�������ǽű��߼�������ȷ�����ݰ�����
						                                                                 int net_protocol::unpack(lua_State*L, const byte* buf, int buf_len, int ext)
						                                                                 {
						                                                                 int top = lua_gettop(L);
						                                                                 int readed_len = 0;
						                                                                 //Э�鲻һ�µĻ�
						                                                                 //assert(_unpack_ref != LUA_NOREF);
						                                                                 //DUMP_STAT_BEGIN

						                                                                 if (_unpack_ref == LUA_NOREF) {
                                                                 print_error(L, "no unpack function. id:
						                                                                 %d\n", _id);
						                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
					                                                                 }
						                                                                 net_protocol::_total_unpack_count++ ;
						                                                                 _unpack_count++ ;

						                                                                 lua_getref(L, _unpack_ref);
						                                                                 assert(lua_isfunction(L, -1));
						                                                                 // ѹ���������Զ�̺�����Ĭ�ϲ���
						                                                                 lua_pushnumber(L, ext);

						                                                                 // ����Զ�̲���ѹջ
						                                                                 for (size_t i=0; i<_args.size(); ++i) {
						                                                                 assert(_args[i]);
						                                                                 int cur_readed = _args[i]->unpack(L, buf+readed_len, buf_len-readed_len);
						                                                                 if (cur_readed == 0) {
                                                                 print_error(L, "unpack argument fail. id:
                                                                 %d, seq:
                                                                 %d, type:
						                                                                 %s\n",
						                                                                 _id, i, _args[i]->get_type());
						                                                                 lua_settop(L, top);
						                                                                 //���ʱ���ݰ���ʽ����
						                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
					                                                                 }
						                                                                 readed_len += cur_readed;
					                                                                 }

						                                                                 //DUMP_STAT_END("unpack id=%d,count=%d", _id, _unpack_count)

						                                                                         // ����Զ�̺���,��������Ϊ Ĭ�ϲ�����Զ�̲�������
						                                                                         int status ;
						                                                                         //DUMP_STAT_BEGIN
						                                                                         status = lua_pcall(L, static_cast<int>(_args.size()+1), 0, 0);
						                                                                         //DUMP_STAT_END("PtoCall id=%d,count=%d", _id, _unpack_count)
						                                                                                 if (status) {
						                                                                                 print_error(L, "run lua function error!\n");
						                                                                                 lua_settop(L, top);
						                                                                                 //�������ݰ��߼�����
						                                                                                 return XYNET_ERROR_UNPACK_SCRIPT ;
					                                                                                 }
						                                                                                 else {
						                                                                                 lua_settop(L, top);
						                                                                                 return readed_len; //�ض����� 0
					                                                                                 }
					                                                                                 }

						                                                                                 int net_protocol::get_check_id(void)
						                                                                                 {
						                                                                                 std::string buf(_pack_func_name);
						                                                                                 for (size_t i=0; i<_args.size(); ++i) {
						                                                                                 buf += _args[i]->get_type();
						                                                                                 buf += _args[i]->get_hashstr();
					                                                                                 }
						                                                                                 return calc_hashnr(buf.c_str(), buf.length());
					                                                                                 }

						                                                                                 //////////////////////////////////////////////////////////////////////////
						                                                                                 // Э�������
						                                                                                 std::vector<net_protocol*> proto_manager::_s_protos ;
						                                                                                 send_hook_t proto_manager::s_data_sender = NULL;
						                                                                                 unsigned proto_manager::_static_protocol_count = 0 ;

						                                                                                 proto_manager::proto_manager(send_hook_t func)
						                                                                                 {
						                                                                                 s_data_sender = func;
						                                                                                 fcall_arg_manager::init();
						                                                                                 // ���һ��Ĭ��Э�飬ռ�ñ��0
						                                                                                 if (_s_protos.size() == 0) {
						                                                                                 _s_protos.push_back(new net_protocol(0));
					                                                                                 }
					                                                                                 }

						                                                                                 proto_manager::~proto_manager()
						                                                                                 {
						                                                                                 for (size_t i = 0; i<_s_protos.size(); ++i) {
						                                                                                 if (_s_protos[i]) {
						                                                                                 delete _s_protos[i];
					                                                                                 }
					                                                                                 }
						                                                                                 _s_protos.clear();
						                                                                                 fcall_arg_manager::destruct();
					                                                                                 }

						                                                                                 //�ڽű������ͳ����Ϣ
						                                                                                 int proto_manager::stat (lua_State * L)
						                                                                                 {
						                                                                                 lua_newtable (L) ;
						                                                                                 lua_pushstring (L, "total_pack_count") ;
						                                                                                 lua_pushinteger (L, net_protocol::_total_pack_count) ;
						                                                                                 lua_settable (L, -3) ;
						                                                                                 for (size_t i = 0; i<_s_protos.size(); ++i) {
						                                                                                 lua_pushinteger (L, _s_protos[i]->get_id()) ;
						                                                                                 lua_pushinteger (L, _s_protos[i]->_pack_count) ;
						                                                                                 lua_settable (L, -3) ;
					                                                                                 }

						                                                                                 lua_newtable (L) ;
						                                                                                 lua_pushstring (L, "total_unpack_count") ;
						                                                                                 lua_pushinteger (L, net_protocol::_total_unpack_count) ;
						                                                                                 lua_settable (L, -3) ;
						                                                                                 for (size_t i = 0; i<_s_protos.size(); ++i) {
						                                                                                 lua_pushinteger (L, _s_protos[i]->get_id()) ;
						                                                                                 lua_pushinteger (L, _s_protos[i]->_unpack_count) ;
						                                                                                 lua_settable (L, -3) ;
					                                                                                 }

						                                                                                 return 2 ;
					                                                                                 }

						                                                                                 int proto_manager::pack_protocol(lua_State *L)
						                                                                                 {
						                                                                                 if ( !lua_isfunction(L, 1) )
						                                                                                 return 0;
						                                                                                 if (lua_getupvalue(L, 1, 1) == NULL )
						                                                                                 return 0;
						                                                                                 net_protocol *protocol_obj = static_cast<net_protocol*>(lua_touserdata(L, -1));
						                                                                                 byte * data = (byte*)db_buf_alloc_pack(g_out_buf, MAXTONETD) ;
						                                                                                 if (data == NULL)
						                                                                                 {
						                                                                                 //_RUNTIME_ERROR("%s %d pack_protocol error! _alloc_packMbufAllocPack return NULL", __FILE__, __LINE__);
						                                                                                         return 0;
					                                                                                         }
						                                                                                         int len = protocol_obj -> marshal (L, data, MAXTONETD) ;
						                                                                                         lua_pushlstring(L, (char*)data, len);
						                                                                                         return 1;
					                                                                                         }

						                                                                                         int proto_manager::send_pack_data(lua_State *L)
						                                                                                         {
						                                                                                         VFD Vfd = (VFD)luaL_checknumber(L, 1);
						                                                                                         size_t len = 0;
						                                                                                         const char* data = luaL_checklstring(L, 2, &len);
						                                                                                         if (data == NULL || len == 0)
						                                                                                         return 0;
						                                                                                         byte * buf = (byte*)db_buf_alloc_pack(g_out_buf, MAXTONETD) ;
						                                                                                         if (buf == NULL)
						                                                                                         {
						                                                                                         //_RUNTIME_ERROR("%s %d send_pack_data error! db_buf_alloc_pack return NULL", __FILE__, __LINE__);
						                                                                                                 return 0;
					                                                                                                 }
						                                                                                                 memcpy(buf, data, len);
						                                                                                                 proto_manager::s_data_sender(buf, len, Vfd, 0, 0);
						                                                                                                 return 0;
					                                                                                                 }

						                                                                                                 int proto_manager::add_arg_type(lua_State* L)
						                                                                                                 {
						                                                                                                 fcall_arg_manager::add_table_args(L);
						                                                                                                 return 0;
					                                                                                                 }

						                                                                                                 // �����ﴦ��Ԥ����Э���
						                                                                                                 int proto_manager::add_static_protocol (lua_State *L)
						                                                                                                 {
						                                                                                                 int ret = add_protocol(L) ;
						                                                                                                 if (1 == ret)
						                                                                                                 _static_protocol_count++ ;

						                                                                                                 return ret ;
					                                                                                                 }

						                                                                                                 int proto_manager::add_protocol(lua_State* L)
						                                                                                                 {
						                                                                                                 // ����Э�����˳���Զ�����Э��ID
						                                                                                                 assert(_s_protos.size() > 0); // ����ʹ��Э���0

						                                                                                                 unsigned int proto_id = _s_protos.size();
						                                                                                                 net_protocol* proto = new net_protocol(proto_id);
						                                                                                                 if (proto->load(L) == PROTO_OK) {
						                                                                                                 _s_protos.push_back(proto);
						                                                                                                 lua_pushnumber(L, proto_id);
						                                                                                                 return 1;
					                                                                                                 }
						                                                                                                 else {
						                                                                                                 lua_pushstring(L, "load protocol failed!") ;
						                                                                                                 lua_pushboolean(L, 0) ;
                                                                                                 raise_error(L, "[proto_manager]:
							                                                                                                 load protocol failed!");
							                                                                                                 return 2;
						                                                                                                 }
						                                                                                                 }

							                                                                                                 int proto_manager::update_protocol(lua_State* L)
							                                                                                                 {
							                                                                                                 int top = lua_gettop(L);
							                                                                                                 if ((top >= 2) && lua_isstring(L, -1) && lua_isnumber(L, -2)) {
							                                                                                                 unsigned int proto_id = (unsigned int)lua_tonumber(L, -2);
							                                                                                                 if (proto_id>=0 && proto_id<_s_protos.size()) {
							                                                                                                 assert(_s_protos[proto_id]);
							                                                                                                 net_protocol* proto = new net_protocol(proto_id);
							                                                                                                 if (proto->load(L) == PROTO_OK) {
							                                                                                                 delete _s_protos[proto_id];
							                                                                                                 _s_protos[proto_id] = proto;
							                                                                                                 lua_pushnumber(L, proto_id);
							                                                                                                 return 1;
						                                                                                                 }
							                                                                                                 else {
							                                                                                                 delete proto;
							                                                                                                 lua_pushstring(L, "update protocol failed!") ;
							                                                                                                 lua_pushboolean(L, 0) ;
                                                                                                 raise_error(L, "[proto_manager]:
							                                                                                                 update protocol failed!");
							                                                                                                 return 2;
						                                                                                                 }
						                                                                                                 }
						                                                                                                 }
							                                                                                                 return 0;
						                                                                                                 }

							                                                                                                 // ��������������ݣ�����ʵ�ʽ����ĳ���
							                                                                                                 // ����ֵ������� 0
							                                                                                                 int proto_manager::unpack_data(lua_State*L, const byte* buf, int buf_size, int ext)
							                                                                                                 {
							                                                                                                 try
							                                                                                                 {
							                                                                                                 // ����Э����
							                                                                                                 int readed_len = 1;
							                                                                                                 check_buf(readed_len, buf_size);
							                                                                                                 unsigned int proto_id = 0;
							                                                                                                 memcpy(&proto_id, buf, 1);

							                                                                                                 if (proto_id == 0) {
							                                                                                                 proto_id = 0;
							                                                                                                 readed_len += 2;
							                                                                                                 check_buf(readed_len, buf_size);
							                                                                                                 memcpy(&proto_id, buf+1, 2);
						                                                                                                 }
							                                                                                                 if (proto_id >= _s_protos.size()) {
                                                                                                 print_error(L, "protocol id:
							                                                                                                 %d overflow\n", proto_id);
							                                                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
						                                                                                                 }

							                                                                                                 //VFDCtl * ctl = VfdMbuf.Get(ext) ;
							                                                                                                 net_protocol* proto = _s_protos[proto_id];
							                                                                                                 /*
							                                                                                                 if (ctl)
							                                                                                                 {
							                                                                                                 bool Logined = ctl->GetLogined();

							                                                                                                 //��¼û�������ǰ��Э���ܵ����ơ�
							                                                                                                 if (!Logined )
							                                                                                                 {
							                                                                                                 lua_getglobal(L, "CheckLoginProt");
							                                                                                                 lua_pushstring(L, proto->_pack_func_name.c_str());
							                                                                                                 lua_btcall(L, 1, 1, 0);
							                                                                                                 bool ret = lua_toboolean(L, -1);
							                                                                                                 if (!ret)
							                                                                                                 {
                                                                                                 print_error(L, "protocol sequence error:
							                                                                                                 %s\n", proto->_pack_func_name.c_str());
							                                                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
						                                                                                                 }
						                                                                                                 }
						                                                                                                 }
							                                                                                                 */

							                                                                                                 if (proto) {
                                                                                                 //print_error(L, "[proc proto]:
							                                                                                                 %s", proto->_pack_func_name.c_str());
							                                                                                                 int cur_readed = proto->unpack(L, buf+readed_len, buf_size-readed_len, ext);
							                                                                                                 if (cur_readed < 0)  //����
							                                                                                                 return cur_readed ;
							                                                                                                 if (cur_readed == 0 && proto->_args.size () > 0) {
                                                                                                 print_error(L, "unpack failed:
							                                                                                                 %s\n", proto->_pack_func_name.c_str());
							                                                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
						                                                                                                 }
							                                                                                                 readed_len += cur_readed;
							                                                                                                 return readed_len;
						                                                                                                 }
							                                                                                                 else {
							                                                                                                 print_error(L, "unpack failed, invalid protocol id\n");
							                                                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
						                                                                                                 }
						                                                                                                 }
							                                                                                                 catch (...)
							                                                                                                 {
							                                                                                                 print_error(L, "unpack failed, crash in proto unpack!\n");
							                                                                                                 return XYNET_ERROR_UNPACK_FORMAT ;
						                                                                                                 }
						                                                                                                 }

							                                                                                                 int proto_manager::get_ptos_checkid(lua_State*L)
							                                                                                                 {
							                                                                                                 int* buf = new int[_s_protos.size()];
							                                                                                                 for (size_t i=0; i<_s_protos.size(); ++i) {
							                                                                                                 buf[i] = (_s_protos[i])?_s_protos[i]->get_check_id():0;
                                                                                                 //printf("i:
							                                                                                                 %d, %d, %s\n",i, buf[i], _s_protos[i]->_pack_func_name.c_str());
						                                                                                                 }
							                                                                                                 lua_pushnumber(L, calc_hashnr((const char*)buf, sizeof(int)*_s_protos.size()));
							                                                                                                 delete [] buf;
							                                                                                                 return 1;
						                                                                                                 }
