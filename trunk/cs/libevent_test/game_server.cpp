// c++ header
#include <iostream>
#include <queue>
using namespace std;

// system header
extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
}

// -----------------------------------------------------------------
// signal handle
// -----------------------------------------------------------------
void waitChild(int sig)
{
	cout<<"wait child"<<endl;
	waitpid(-1, NULL, WNOHANG);
}

// -----------------------------------------------------------------
// message
// -----------------------------------------------------------------
#define MSG_COMMON	0
#define MSG_HUGE	1
#define MSG_QIAN	2
struct Msg {
	int _type;
};

struct Client {
	int _sock;
	time_t _active;
};

struct Event {
	int _event;
	int _hid;
	char _data[10];
};

#define NET_NEW 0
#define NET_DATA 1
#define NET_LEAVE 2
#define NET_TIMER 3

// -----------------------------------------------------------------
// server
// -----------------------------------------------------------------
class Server
{
private:
	int _listensock;
	struct sockaddr_in _saddr;
	short _port;
	int _listenLen;
	//vector<Client> _clients;
	Client _clients[1024];
	queue<Event> _queue;
	int _count;
	float _timeout;
	int _timeslap;
	int _period;

	char _recvbuf[100];
public:
	Server(short port, int listenLen) {
		_timeout = 70.0;
		_timeslap = long(time(0) * 1000);
		_period = 1000;

		_count = 0;
		_port = port;
		_listenLen = listenLen;
		_listensock = socket(PF_INET, SOCK_STREAM, 0);

		if (_listensock == -1) {
			perror("socket");
			exit(1);
		}

		int opt = SO_REUSEADDR;

		if (setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			perror("setsockopt");
			exit(1);
		}

		setNoBlocking(_listensock);
		cout<<_listensock<<endl;

		for (int i=0; i<1024; i++) {
			_clients[i]._sock = -1;
		}
	}

	~Server() {
		close(_listensock);
	}

private:
	void setNoBlocking(int sock) {
		if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
			perror("fcntl");
			exit(1);
		}
	}

	void doBind() {
		bzero(&_saddr, sizeof(struct sockaddr_in));
		_saddr.sin_port = htons(_port);
		_saddr.sin_family = AF_INET;
		_saddr.sin_addr.s_addr = htons(INADDR_ANY);

		if ( bind(_listensock, (struct sockaddr*)&_saddr, sizeof(struct sockaddr)) == -1 ) {
			perror("bind");
			exit(1);
		}
	}

	void doListen() {
		if (listen(_listensock, _listenLen) == -1) {
			perror("listen");
			exit(1);
		}
	}

public:
	void process() {
		time_t current = time(0);
		int fd = accept(_listensock, NULL, NULL);
		cout<<"fd = "<<fd<<endl;

		if (fd < 0) {
			perror("accept");
		} else {
			cout<<"new connect"<<endl;
			setNoBlocking(fd);
			cout<<"after set"<<endl;

			if (_count > 1024) {
				close(fd);
			} else {
				int pos = -1;

				for (int i=0; i<1024; i++) {
					if (_clients[i]._sock == -1) {
						pos = i;
						break;
					}
				}

				if (pos == -1) {
					close(fd);
				} else {
					Client cc;
					cc._sock = fd;
					cc._active = current;
					_clients[pos] = cc;
					_count++;
					Event node;
					node._event = NET_NEW;
					node._hid = pos;
					//memcpy(node._data, "new", 3);
					_queue.push(node);
				}
			}
		}

		for (int i=0; i<1024; i++) {
			if (_clients[i]._sock == -1) {
				continue;
			}

			while (1) {
				int nread = recv(_clients[i]._sock, _recvbuf, 100, 0);//no blocked socket

				if (nread == 0) {
					break;
				}

				Event node;
				//memcpy(node._data, _recvbuf, nread);
				_queue.push(node);
				_clients[i]._active = current;
			}

			time_t timeout = current - _clients[i]._active;

			if (timeout > _timeout) { //ÅÐ¶ÏÊÇ·ñ³¬Ê±
				_clients[i]._sock = -1;
				Event node;
				node._event = NET_LEAVE;
				node._hid = i;
				//memcpy(node._data, " ", 1);
				_queue.push(node);
			}
		}

		current = long(time(0) * 1000);

		if (current - _timeslap > 100000) { // ????
			_timeslap = current;
		}

		while (_timeslap < current) {
			//cout<<"while"<<endl;
			Event node;
			node._event = NET_TIMER;
			node._hid = 0;
			//memcpy(node._data, " ", 1);
			_queue.push(node);
			_timeslap += _period;
		}
	}

	int doSend(int hid, char* data, int len) {
		if (hid <0 or hid > 1024) {
			return -1;
		}

		send(_clients[hid]._sock, data, len, 0);
	}

	Event doRead() {
		if (_queue.empty()) {
			Event ee;
			ee._event = -1;
			ee._hid = -1;
			return ee;
		}

		Event node = _queue.front();
		_queue.pop();
		return node;
	}
// -----------------------------------------------------------------
// interface
// -----------------------------------------------------------------
public:
	void startUp() {
		cout<<"start up"<<endl;
		doBind();
		doListen();
	}
};


// -----------------------------------------------------------------
// main test
// -----------------------------------------------------------------
int main()
{
	Server ss = Server(2000, 5);
	ss.startUp();

	while (1) {
		sleep(1);
		ss.process();
		Event ee = ss.doRead();
		cout<<"event type = "<<ee._event<<endl;
	}

	return 0;
}
