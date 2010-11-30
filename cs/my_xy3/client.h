#ifndef _CLIENT_H
#define _CLIENT_H

#include "databuf.h"

typedef unsigned char byte;

// �ͻ���
class Client
{
public:
	int _sock;
private:
	DataBuf _rbuf;
	DataBuf _wbuf;
	int _state;
	int _errc;
public:
	//ֻ��server���õ�������
	time_t _active;
	int _hid;
public:
	Client();
	~Client();
//private:
public:
	int tryConnect();
	int tryRecv();
	int trySend();
	void setNoBlocking(int sock);
	void out(int code, char* msg);
public:
	int doConnect(const char* ip, int port);
	int doClose();
	void assign(int sock);
	int getSock();
	int process();
	int getState();
	int doSend(byte* data, int len);
	char* doRecv(short& size);
};

#endif
