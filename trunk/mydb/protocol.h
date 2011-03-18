// $Id: protocol.hpp 64018 2008-11-26 15:08:53Z tony $

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "data_format.h"
#include <string>
#include <vector>
using namespace std;

#define XYNET_ERROR_UNPACK_FORMAT	-1  //���ʱ���ݰ���ʽ����
#define XYNET_ERROR_UNPACK_SCRIPT	-2  //�ű�������ȷ�����ݰ�����


extern std::string cur_char;
//////////////////////////////////////////////////////////////////////////
// Զ�̵��ø�ʽ����
//////////////////////////////////
class proto_manager ;

class net_protocol
{
	std::vector<fcall_base_arg*> _args ; // �����б�
	std::string _pack_func_name ;// ����ӿ����ƣ�������ʵ��
	int _unpack_ref ;			 // ����ӿ����ã��ű���ʵ��
	int _id ;					 // Э��id���й������Զ�����
	bool _is_maker;				 // �Ƿ�ΪԶ�̺���������
	friend class proto_manager ;
protected:
	int process_type(lua_State *L, const char* path);
	int convert_protocol(lua_State *L) ;
	int func_proc(lua_State *L) ;
	int arglist_proc(lua_State *L) ;
public:
	size_t _pack_count ;  			//����pack_func �Ĵ���������ͳ�ơ�
	size_t _unpack_count ;  		//����unpack_func �Ĵ���������ͳ�ơ�
	static size_t _total_pack_count ;
	static size_t _total_unpack_count ;

	net_protocol(int id) ;
	~net_protocol() ;
	int get_id () {
		return _id ;
	}

	int load(lua_State*L) ; //  ����Э���ʽ��ջ��ΪЭ���ʽ�����ļ�·��
	int unpack(lua_State*L, const byte* buf, int buf_len, int ext) ;
	int get_check_id(void);
	int marshal(lua_State * L, byte * buf, size_t buf_len) ;

protected:
	static int pack(lua_State* L) ; // ���Э���������Զ�ע����lua������й�Զ�̵�����ʹ��

};

//////////////////////////////////////////////////////////////////////////
// Э�������
////////////////////
// �������ݷ��͵Ļص�����
typedef int (*send_hook_t)(const byte* data, int data_len, int ext, int ismulticast, int mcpayloadlen);

class proto_manager
{
	static std::vector<net_protocol*> _s_protos;
	static send_hook_t s_data_sender;
	static unsigned _static_protocol_count ;  //��̬��Э��ĸ���
	friend class net_protocol;
public:
	proto_manager(send_hook_t func);
	~proto_manager();

	static int stat (lua_State * L) ;

	// ���������ӿ�
	int unpack_data(lua_State*L, const byte* buf, int buf_size, int ext);// ��������������ݣ�����ʵ�ʽ����ĳ���

	// ����Ľű��ӿ�
	static int add_arg_type(lua_State*L); // ջ��Ϊ���ݸ�ʽ�����ļ�·��
	static int add_protocol(lua_State*L); // ջ��ΪЭ���ʽ�����ļ�·��
	static int update_protocol(lua_State*L); // ����Э��
	static int add_static_protocol(lua_State*L); //ֻ��add_protocol�ļ򵥷�װ
	static int get_ptos_checkid(lua_State*L); // ��ȡ�����Э���У����

	static int pack_protocol(lua_State*L);//��Э�����ݴ���������ظ��ű�
	static int send_pack_data(lua_State*L); //ֱ�ӷ����Ѿ���ð�������
};

#define MAXTONETD  			0x8000  //�������洫���netd��������ݰ�����
#endif //_PROTOCOL_H
