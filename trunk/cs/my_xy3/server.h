#ifndef _SERVER_H
#define _SERVER_H

#include "event.h"
//#include "client.h"
#include <vector>
#include <queue>
#include <map>
#include "op_epoll.h"
using namespace std;

typedef unsigned char byte;

// ¿‡«∞…Í√˜
class Client;

const int MAX_CLIENTS = 1024;

// -----------
// server
// -----------
class Server
{
private:
	int _state;
	Client** _clients;
	int _max;
	int _index;
	queue<Event*> _queue;
	int _count;
	int _listenSock;
	short _port;
	float _timeout;
	int _timeslap;
	int _period;
	EpollOp* _op;
	map<int, Client*> _fc;
	map<int, int> _fpos;
public:
	Server(short port);
	~Server();
private:
	void setNoBlocking(int sock);
	void out(int status, const char* msg);
public:
	void shutDown();
	void process();
	int doSend(int hid, byte* data, int len);
	int doBroadcast(byte* data, int len);
	Event* doRead();
	void startUp();
	void setTimer(int millisec);
	int dispatch();
};

#endif
