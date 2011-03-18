// c++ header
#include <iostream>
#include <queue>
#include <map>
using namespace std;

// system header
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// �Զ���
#include "server.h"
#include "client.h"



// -----------------------------------------------------------------
// server
// -----------------------------------------------------------------
Server::Server(short port)
{
	_state = 0;
	_count = 0;
	_listenSock = -1;
	_port = -1;
	_timeout = 70;//0.07;
	_timeslap = long(time(0) * 1000);
	_period = 5000;// �൱��5����һ��timer
	_port = port;

	_op = new EpollOp();
	_fc.clear();
	_max = _op->_rt.rlim_max;
	_clients = new Client*[_max];
}

Server::~Server()
{
	if (_listenSock != -1) {
		close(_listenSock);
	}

	// �������
	for (int i=0; i<_max; i++)
		if (_clients[i] != NULL) {
			delete _clients[i];
		}

	while (!_queue.empty()) {
		Event* p = _queue.front();
		_queue.pop();
		delete p;
	}
}

void Server::setNoBlocking(int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(1);
	}
}

void Server::shutDown()
{
	if (_listenSock != -1) {
		close(_listenSock);
	}

	_listenSock = -1;

	for (int i=0; i<_max; i++) {
		_clients[i] = NULL;
	}

	while (_queue.empty()) {
		_queue.pop();
	}

	_state = 0;
	_count = 0;
}

void Server::out(int status, const char* msg)
{
	perror(msg);
	exit(status);
}

void Server::startUp()
{
	shutDown();
	_listenSock = socket(PF_INET, SOCK_STREAM, 0);

	if (_listenSock == -1) {
		out(1, "socket");
	}

	setNoBlocking(_listenSock);
	int opt = 1;//�����ʾ���ø�ѡ��, 0 ��ʾ������
	unsigned int size = sizeof(opt);
	setsockopt(_listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, size);

	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_port = htons(_port);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htons(INADDR_ANY);

	if ( bind(_listenSock, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1 ) {
		out(1, "bind");
	}

	if (listen(_listenSock, 5) == -1) {
		perror("listen");
		exit(1);
	}

	_state = 1;// ״̬
	_timeslap = long(time(0) * 1000); // ����ʱ���

	_op->addFd(_listenSock, EV_READ);
}

void Server::process()
{
	time_t current = time(0);

	if (_state != 1) {
		return;
	}

	int fd = accept(_listenSock, NULL, NULL);

	if (fd < 0) {
		perror("accept");
	} else {
		setNoBlocking(fd);

		if (_count > 1024) {
			close(fd);
		} else {
			int pos = -1;

			for (int i=0; i<_max; i++) {
				if (_clients[i] == NULL) {
					pos = i;
					break;
				}
			}

			if (pos == -1) {
				close(fd);
			} else {
				// ��ջ�����ö�
				Client* cc = new Client();
				cc->assign(fd);
				cc->_active = current;// �洢activeʱ��
				cc->_hid = pos; // �洢λ��

				_clients[pos] = cc;
				_count++;

				Event* node = new Event();
				node->_event = NET_NEW;
				node->_hid = pos;
				_queue.push(node);
				cout<<"new"<<endl;
			}
		}
	}

	//�������б����client
	for (int i=0; i<_max; i++) {
		if (_clients[i] == NULL) {
			continue;
		}

		// client�ķ��ͺͽ��մ���
		_clients[i]->process();

		// ��ȡ_clients[i] �������������ݰ�
		while (1) {
			short packSize;
			// packDataָ��ѣ��õ�֮��Ҫdelete
			char* packData = _clients[i]->doRecv(packSize);//��ȡһ���������ݰ�

			if (packData == NULL || packSize == 0) {
				break;
			}

			Event* node = new Event();
			node->_event = NET_DATA;
			node->_hid = _clients[i]->_hid;
			node->addData(packData, packSize);//memcpy
			delete packData;// �����
			_queue.push(node);
			cout<<"data"<<endl;
			_clients[i]->_active = current;
		}

		time_t timeout = current - _clients[i]->_active;//��ǰcurrent���client��һ�ν�������ʱ���ʱ��

		if (timeout > _timeout || _clients[i]->getState() == 0) {//�ж��Ƿ�ʱ timeout > 0.07
			cout<<"leave"<<endl;
			Event* node = new Event();
			node->_event = NET_LEAVE;
			node->_hid = _clients[i]->_hid;
			_queue.push(node);

			if (_clients[i] != NULL) {
				delete _clients[i];
				_clients[i] = NULL;
				_count--;
			}

			close(_clients[i]->getSock());// close socket
		}
	}

	current = long(time(0) * 1000);

	if (current - _timeslap > 100000) { //
		_timeslap = current;
	}

	if (_period > 0) {
		while (_timeslap < current) { //��֤ÿ��periodʱ����һ��NET_TIMER�¼�
			Event* node = new Event();
			node->_event = NET_TIMER;
			node->_hid = 0;
			_queue.push(node);
			_timeslap += _period;//ÿ��_period�����һ��TIMER�¼�
			cout<<"timer"<<endl;
		}
	}

	usleep(500000);
}

void Server::setTimer(int millisec)
{
	_period = millisec;
	_timeslap = long(time(0) * 1000);
}


int Server::doSend(int hid, byte* data, int len)
{
	if (hid <0 or hid > 1024) {
		return -1;
	}

	if (_clients[hid] == NULL) {
		return -1;
	}

	_clients[hid]->doSend(data, len);
}

Event* Server::doRead()
{
	if (_queue.empty()) {
		return NULL;
	}

	Event* p = _queue.front();
	_queue.pop();
	return p;
}

int Server::doBroadcast(byte* data, int len)
{
	for (int i=0; i<_max; i++) {
		if (_clients[i] != NULL) {
			_clients[i]->doSend(data, len);
		}
	}
}

int Server::dispatch()
{
	time_t current = time(0);
	std::cout<<"_count = "<<_count<<std::endl;
	int ret = _op->doDispatch();// epoll_wait

	if (ret == -1) {
		perror("epoll_wait");
		return -1;
	}

	//�пͻ������ӶϿ�����ʹepoll_wait���أ�
	//������whileѭ����sleep������ᵼ�������ӶϿ�ʱ���ô���sleep�ڼ䣬������һ��epoll_waitʱ����
	for (int i=0; i<ret; i++) {
		if (_op->_events[i].data.fd == _listenSock) {
			int new_fd = accept(_listenSock, NULL, NULL);
			cout<<"new_fd = "<<new_fd<<endl;

			if (new_fd < 0) {
				perror("accept");
				continue;
			} else {
				if (_count >= _max) {
					close(new_fd);
				} else {
					setNoBlocking(new_fd);
					int pos = -1;

					for (int i=0; i<_max; i++) {
						if (_clients[i] == NULL) {
							pos = i;
							break;
						}
					}

					if (pos == -1) {
						close(new_fd);
					} else {
						int ret = _op->addFd(new_fd, EV_READ);

						if (ret < 0) {
							close(new_fd);
							continue;
						} else if (ret == 0) {
							Client* cc = new Client();
							cc->assign(new_fd);
							cc->_active = current;
							cc->_hid = pos;
							_clients[pos] = cc;
							_count++;
							_fpos[new_fd] = pos; // ��fdΪkey����¼��pos���Ա��ҵ�_client
							Event* node = new Event();
							node->_event = NET_NEW;
							node->_hid = pos;
							_queue.push(node);
							cout<<"[new]"<<endl;
						}
					}
				}
			}
		} else {// connection socket fd
			int old_fd = _op->_events[i].data.fd;
			cout<<"old_fd = "<<old_fd<<endl;
			int pos = _fpos[old_fd];
			Client* ct = _clients[pos];

			if (!ct) {
				continue;
			}

			ct->tryRecv();
			ct->trySend();

			while (1) {
				short packSize;
				char* packData = ct->doRecv(packSize);//packDataָ��ѣ��õ�֮��Ҫdelete

				if (packData == NULL || packSize == 0) {
					break;
				}

				Event* node = new Event();
				node->_event = NET_DATA;
				node->_hid = ct->_hid;
				node->addData(packData, packSize);//memcpy
				delete packData;// �����
				_queue.push(node);
				cout<<"[data]"<<endl;
				ct->_active = current;
			}

			if (ct->getState() == 0) {//�жϿ�
				int ret = _op->rmFd(old_fd, NULL);

				if (ret == 0) {
					//close(old_fd);
					cout<<"[leave]"<<endl;
					Event* node = new Event();
					node->_event = NET_LEAVE;
					node->_hid = ct->_hid;
					_queue.push(node);
					delete ct;//����close
					_clients[pos] = NULL;
					_fpos.erase(old_fd);
					_count--;
				} else if (ret < 0) {
					std::cout<<"wrong old_fd = "<<old_fd<<std::endl;
				}
			}
		}
	}
}
