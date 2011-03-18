// $Id: protocol.hpp 64018 2008-11-26 15:08:53Z tony $

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "data_format.h"
#include <string>
#include <vector>
using namespace std;

#define XYNET_ERROR_UNPACK_FORMAT	-1  //解包时数据包格式出错
#define XYNET_ERROR_UNPACK_SCRIPT	-2  //脚本处理正确的数据包出错


extern std::string cur_char;
//////////////////////////////////////////////////////////////////////////
// 远程调用格式描述
//////////////////////////////////
class proto_manager ;

class net_protocol
{
	std::vector<fcall_base_arg*> _args ; // 参数列表
	std::string _pack_func_name ;// 打包接口名称，引擎内实现
	int _unpack_ref ;			 // 解包接口引用，脚本内实现
	int _id ;					 // 协议id，有管理器自动分配
	bool _is_maker;				 // 是否为远程函数定义者
	friend class proto_manager ;
protected:
	int process_type(lua_State *L, const char* path);
	int convert_protocol(lua_State *L) ;
	int func_proc(lua_State *L) ;
	int arglist_proc(lua_State *L) ;
public:
	size_t _pack_count ;  			//调用pack_func 的次数，用于统计。
	size_t _unpack_count ;  		//调用unpack_func 的次数，用于统计。
	static size_t _total_pack_count ;
	static size_t _total_unpack_count ;

	net_protocol(int id) ;
	~net_protocol() ;
	int get_id () {
		return _id ;
	}

	int load(lua_State*L) ; //  加载协议格式，栈顶为协议格式描述文件路径
	int unpack(lua_State*L, const byte* buf, int buf_len, int ext) ;
	int get_check_id(void);
	int marshal(lua_State * L, byte * buf, size_t buf_len) ;

protected:
	static int pack(lua_State* L) ; // 完成协议打包，被自动注册至lua虚拟机中供远程调用者使用

};

//////////////////////////////////////////////////////////////////////////
// 协议管理器
////////////////////
// 用于数据发送的回调函数
typedef int (*send_hook_t)(const byte* data, int data_len, int ext, int ismulticast, int mcpayloadlen);

class proto_manager
{
	static std::vector<net_protocol*> _s_protos;
	static send_hook_t s_data_sender;
	static unsigned _static_protocol_count ;  //静态的协议的个数
	friend class net_protocol;
public:
	proto_manager(send_hook_t func);
	~proto_manager();

	static int stat (lua_State * L) ;

	// 对外的引擎接口
	int unpack_data(lua_State*L, const byte* buf, int buf_size, int ext);// 传入需解析的数据，返回实际解析的长度

	// 对外的脚本接口
	static int add_arg_type(lua_State*L); // 栈顶为数据格式描述文件路径
	static int add_protocol(lua_State*L); // 栈顶为协议格式描述文件路径
	static int update_protocol(lua_State*L); // 更新协议
	static int add_static_protocol(lua_State*L); //只是add_protocol的简单封装
	static int get_ptos_checkid(lua_State*L); // 获取已添加协议的校验码

	static int pack_protocol(lua_State*L);//将协议数据打包，并返回给脚本
	static int send_pack_data(lua_State*L); //直接发送已经打好包的数据
};

#define MAXTONETD  			0x8000  //允许引擎传输给netd的最大数据包长度
#endif //_PROTOCOL_H
