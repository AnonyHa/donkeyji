#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "client.h"

// �ͻ���
Client::Client()
{
	_sock = -1;
	_state = 0; 
	_errc = 0;
}

Client::~Client()
{
	cout<<"~~~client"<<endl;
	if (_sock != -1) {
		cout<<"_sock = "<<_sock<<endl;
		close(_sock);
	}
}

int Client::tryConnect()
{
	if (_state == 2)//connect���
		return 1;
	if (_state != 1)//connect����˻���δ����connect
		return -1;

	// ����Ϊʲô�أ� ���nonblocking socket��connect��û�����
	char buf;	
	int ret = recv(_sock, &buf, 0, 0);
	if (ret == -1) {//nonblocking�϶���������
		if (errno != EWOULDBLOCK and errno != EINTR) {// ��ʾconnected
			doClose();
			return 0;
		}
		/*
		if (errno == EWOULDBLOCK)// ��ʾ��û�����connect����
		{
			_state = 2;//Ϊ������Ҫֱ2???
			//_rbuf.clear();
			return 1;
		}
		doClose();//��stateΪ0����ʾδ����connect
		return -1;
		*/
	}
	_state = 2; // ��ʾconnect���, state = 2
	return 1;
}

int Client::tryRecv()
{
	char buf[1024]; 
	while (1)
	{
		int ret = recv(_sock, buf, 1024, 0);
		// recv����0��ʾ�Է������Ͽ�����
		if (ret == 0) {
			doClose();
			break;
		}
		// ���Է�û�жϿ�ʱ���Ի᷵��-1
		if (ret == -1) {
			if (errno != EINTR and errno != EWOULDBLOCK) {// ��ʾ��û�����connect����
				doClose(); // ��state = 0����Ϊ�ж϶˿�������
				return -1;
			}
			break;
		}
		if (ret > 0) {
			_rbuf.appendRawData((byte*)buf, ret); // ��ӵ����ջ�������ȥ
			return 0;
		}
	}
}

// -----------------------------------------------
// �����ͻ����������ݷ��ͳ�ȥ,û����������´η�
// -----------------------------------------------
int Client::trySend()
{
	int size = _wbuf.getDataSize();
	if (size == 0)
		return 0;

	int ret = send(_sock, _wbuf.getSendData(), size, 0);

	// �Է��رյ�ʱ�򷵻�-1
	if (ret == -1) {
		perror("send");
		//�⼸��errno֮��ŵ����ǶԷ��رգ����ر�����ʱ��doClose()
		if (errno != EWOULDBLOCK and errno != EINTR and errno != EINPROGRESS) {
			doClose(); // ��state = 0����Ϊ�ж϶˿�������
			return -1;
		}
	}
	//else if (ret == 0)
	//{
		//cout<<"send 0"<<endl;
		//doClose();
		//return 0;
	//}
	else {// ���ͳɹ������û�з�����ɣ������´η���
		//cout<<"send byte = "<<ret<<endl;
		_wbuf.cutData(ret);// ��������ret���ֽ�
		return 0;
	}
}

// -----------------------------------------------
// ��sock����Ϊ������
// -----------------------------------------------
void Client::setNoBlocking(int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(1);
	}
}

// -----------------------------------------------
// ���������Ϣ�����
// -----------------------------------------------
void Client::out(int code, char* msg)
{
	perror(msg);
	exit(code);
}

int Client::doConnect(const char* ip, int port)
{
	_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock == -1)
		out(1, "socket");
	setNoBlocking(_sock);// ��Ϊ������
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);// sin_port��unsigned short�����Ա�����htons()-----
	addr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(_sock, (struct sockaddr*)&addr, sizeof(addr));//nonblocking socket��������
	if (ret == -1) {
		if (errno == EINPROGRESS) {
			cout<<errno<<endl;
			perror("connect");	
		} else {
			perror("connect");	
			exit(1);
		}
	}
	_state = 1;// ��ʾ�Ѿ����ù�connect������û����֤connect�ɹ�

	return 0;
}


// -----------------------------------------------
// �ر�socket
// -----------------------------------------------
int Client::doClose()
{
	_state = 0;
	if (_sock < 0)
		return 0;

	//int ret = close(_sock);//��������close�����������close
	//_sock = -1;// Ϊ����������������close
	return 0;
}

void Client::assign(int sock)
{
	doClose();
	_sock = sock;
	setNoBlocking(_sock);
	int opt = SO_KEEPALIVE;

	// ���� ??????
	setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

	// connect�ɹ���״̬
	_state = 2;
}

int Client::getSock()
{
	return _sock;
}

// -----------------------------------------------
// ÿ֡���ã�����֡��
// -----------------------------------------------
int Client::process()
{
	switch (_state)
	{
	case 0:// ����connect֮ǰ
		return 0;
		break;
	case 1:// �Ѿ�����connect������ȷ���Ƿ��Ѿ�connect��ɣ���Ҫ��tryConnect���м��
		tryConnect();// ʹ��_state == 2
		break;
	case 2:// connect�Ѿ���ɣ����Կ�ʼ�շ����ݰ���
		tryRecv();
		trySend();
		break;
	}
	return 0;
}

int Client::getState()
{
	return _state;
}

// -----------------------------------------------
// ��Ҫ���͵����ݷŵ�������
// -----------------------------------------------
int Client::doSend(byte* data, int len)
{
	// ���
	short head = len;// ʵ��data�����ݳ��ȣ�������head����
	// ����ʽ�������� + ������
	//_wbuf.appendRawData((char*)&head, sizeof(head));// ������
	//_wbuf.appendRawData(data, len);// ������
	return _wbuf.appendPack(data, len);
	//process();// ��ʱ������ ???
}

// -----------------------------------------------
// �ӽ��ջ������ڶ�ȡһ�������İ�, ���ظ�������
// -----------------------------------------------
char* Client::doRecv(short& size)
{
	//process();
	char* p = _rbuf.getPack(size);
	if (p != NULL) {
		char* pack = new char[size];
		if (pack == NULL) {
			cout<<"fail new"<<endl;
			return NULL;
		}
		memcpy(pack, p, size);
		_rbuf.cutData(size + sizeof(short));// ����һ��pack����ͷ+����
		return pack;
	} else
		return NULL;
}
