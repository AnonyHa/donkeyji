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

// 客户端
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
	if (_state == 2)//connect完成
		return 1;
	if (_state != 1)//connect完成了或者未调用connect
		return -1;

	// 这是为什么呢？ 检查nonblocking socket的connect有没有完成
	char buf;	
	int ret = recv(_sock, &buf, 0, 0);
	if (ret == -1) {//nonblocking肯定立即返回
		if (errno != EWOULDBLOCK and errno != EINTR) {// 表示connected
			doClose();
			return 0;
		}
		/*
		if (errno == EWOULDBLOCK)// 表示还没有完成connect操作
		{
			_state = 2;//为何这里要直2???
			//_rbuf.clear();
			return 1;
		}
		doClose();//置state为0，表示未进行connect
		return -1;
		*/
	}
	_state = 2; // 表示connect完成, state = 2
	return 1;
}

int Client::tryRecv()
{
	char buf[1024]; 
	while (1)
	{
		int ret = recv(_sock, buf, 1024, 0);
		// recv返回0表示对方主动断开连接
		if (ret == 0) {
			doClose();
			break;
		}
		// 当对方没有断开时，仍会返回-1
		if (ret == -1) {
			if (errno != EINTR and errno != EWOULDBLOCK) {// 表示还没有完成connect操作
				doClose(); // 将state = 0，作为判断端开的依据
				return -1;
			}
			break;
		}
		if (ret > 0) {
			_rbuf.appendRawData((byte*)buf, ret); // 添加到接收缓冲区中去
			return 0;
		}
	}
}

// -----------------------------------------------
// 将发送缓冲区的内容发送出去,没发完的留着下次发
// -----------------------------------------------
int Client::trySend()
{
	int size = _wbuf.getDataSize();
	if (size == 0)
		return 0;

	int ret = send(_sock, _wbuf.getSendData(), size, 0);

	// 对方关闭的时候返回-1
	if (ret == -1) {
		perror("send");
		//这几种errno之外才当成是对方关闭，当关闭连接时才doClose()
		if (errno != EWOULDBLOCK and errno != EINTR and errno != EINPROGRESS) {
			doClose(); // 将state = 0，作为判断端开的依据
			return -1;
		}
	}
	//else if (ret == 0)
	//{
		//cout<<"send 0"<<endl;
		//doClose();
		//return 0;
	//}
	else {// 发送成功，如果没有发送完成，留着下次发送
		//cout<<"send byte = "<<ret<<endl;
		_wbuf.cutData(ret);// 消耗数据ret个字节
		return 0;
	}
}

// -----------------------------------------------
// 将sock设置为非阻塞
// -----------------------------------------------
void Client::setNoBlocking(int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(1);
	}
}

// -----------------------------------------------
// 处理出错信息的输出
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
	setNoBlocking(_sock);// 设为非阻塞
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);// sin_port是unsigned short，所以必须用htons()-----
	addr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(_sock, (struct sockaddr*)&addr, sizeof(addr));//nonblocking socket立即返回
	if (ret == -1) {
		if (errno == EINPROGRESS) {
			cout<<errno<<endl;
			perror("connect");	
		} else {
			perror("connect");	
			exit(1);
		}
	}
	_state = 1;// 表示已经调用过connect，但还没有验证connect成功

	return 0;
}


// -----------------------------------------------
// 关闭socket
// -----------------------------------------------
int Client::doClose()
{
	_state = 0;
	if (_sock < 0)
		return 0;

	//int ret = close(_sock);//不在这里close，放在外层来close
	//_sock = -1;// 为的是在析构函数中close
	return 0;
}

void Client::assign(int sock)
{
	doClose();
	_sock = sock;
	setNoBlocking(_sock);
	int opt = SO_KEEPALIVE;

	// 心跳 ??????
	setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

	// connect成功的状态
	_state = 2;
}

int Client::getSock()
{
	return _sock;
}

// -----------------------------------------------
// 每帧调用，控制帧率
// -----------------------------------------------
int Client::process()
{
	switch (_state)
	{
	case 0:// 调用connect之前
		return 0;
		break;
	case 1:// 已经调用connect，但不确定是否已经connect完成，需要用tryConnect进行检测
		tryConnect();// 使得_state == 2
		break;
	case 2:// connect已经完成，可以开始收发数据包了
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
// 把要发送的内容放到缓冲区
// -----------------------------------------------
int Client::doSend(byte* data, int len)
{
	// 封包
	short head = len;// 实际data的内容长度，不包括head本身
	// 包格式：包长度 + 包内容
	//_wbuf.appendRawData((char*)&head, sizeof(head));// 包长度
	//_wbuf.appendRawData(data, len);// 包内容
	return _wbuf.appendPack(data, len);
	//process();// 及时处理发送 ???
}

// -----------------------------------------------
// 从接收缓冲区内读取一个完整的包, 返回给调用者
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
		_rbuf.cutData(size + sizeof(short));// 消耗一个pack：包头+包身
		return pack;
	} else
		return NULL;
}
