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

// -----------------------------------------------------------------
// server
// -----------------------------------------------------------------
class Server
{
private:
	int _sock;
	struct sockaddr_in _saddr;
	short _port;
	int _listenLen;
	int _clients[FD_SETSIZE];
	struct fd_set _fdsock;
	struct fd_set _rdsock;
	int _maxfd;
	int _maxi;
public:
	Server(short port, int listenLen) {
		_port = port;
		_listenLen = listenLen;
		_sock = socket(PF_INET, SOCK_STREAM, 0);

		if (_sock == -1) {
			perror("socket");
			exit(1);
		}

		int opt = SO_REUSEADDR;

		if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			perror("setsockopt");
			exit(1);
		}

		setNoBlocking(_sock);
		cout<<_sock<<endl;
		FD_ZERO(&_fdsock);
		FD_SET(_sock, &_fdsock);
		//signal(SIGCHLD, waitChild);// for signal
		_maxfd = _sock;

		for (int i=0; i<FD_SETSIZE; i++) {
			_clients[i] = -1;
		}

		_maxi = -1;
	}

	~Server() {
		close(_sock);
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

		if ( bind(_sock, (struct sockaddr*)&_saddr, sizeof(struct sockaddr)) == -1 ) {
			perror("bind");
			exit(1);
		}
	}

	void doListen() {
		if (listen(_sock, _listenLen) == -1) {
			perror("listen");
			exit(1);
		}
	}

	void doAccept() {
		while (1) {
			int fd = accept(_sock, NULL, NULL);

			if (fd == -1) {
				perror("my accept");
				continue;
			} else {
				cout<<"get a connection"<<endl;
				int pid = fork();

				if (pid < 0) {
					perror("fork");
					continue;
				} else if (pid == 0) {
					Msg mm;// = Msg(MSG_HUGE);
					mm._type = MSG_HUGE;
					int len = send(fd, &mm, sizeof(mm), 0);

					if (len == -1) {
						perror("send");
						exit(1);
					}

					close(fd);//close in child
					exit(1);//send SIGCHLD
				} else {
					cout<<"parent"<<endl;
					close(fd);// close in parent
					continue;
				}
			}
		}
	}


	void doSelect() {
		cout<<"doselect"<<endl;

		while (1) {
			_rdsock = _fdsock;
			int nready = select(_maxfd+1, &_rdsock, NULL, NULL, NULL);//(struct timeval*)&_tv);

			if (nready <= 0) {
				perror("select");
				continue;
			}

			if (FD_ISSET(_sock, &_rdsock)) {
				int fd = accept(_sock, NULL, NULL);

				if (fd < 0) {
					perror("accept");
					exit(1);
				}

				cout<<"new connection"<<endl;
				setNoBlocking(fd);
				int i;

				//找到一个空位来存放fd
				for (i=0; i<FD_SETSIZE; i++) {
					if (_clients[i] < 0) {
						_clients[i] = fd;
						break;
					}
				}

				if (i == FD_SETSIZE) {
					cout<<"too many clients"<<endl;
				}

				FD_SET(fd, &_fdsock);

				if (fd > _maxfd)	{
					_maxfd = fd;
				}

				if (i > _maxi)	{
					_maxi = i;
				}

				if (--nready < 0) {
					continue;
				}
			}

			for (int i=0; i<_maxi; i++) {
				if (FD_ISSET(_clients[i], &_rdsock)) {
					Msg mm;
					int ret = recv(_clients[i], &mm, sizeof(mm), 0);

					if (ret < 0) {
						close(_clients[i]);
						FD_CLR(_clients[i], &_fdsock);
						_clients[i] = -1;
						perror("recv");
						//exit(1);
					} else if (ret == 0) {
						continue;
					} else {
						//close(_clients[i]);
						//FD_CLR(_clients[i], &_fdsock);
						//_clients[i] = -1;
						cout<<mm._type<<endl;
					}

					if (--nready < 0)	{
						break;
					}
				}
			}
		}
	}

// -----------------------------------------------------------------
// interface
// -----------------------------------------------------------------
public:
	void startUp() {
		cout<<"start up"<<endl;
		doBind();
		doListen();
		//doAccept();
		doSelect();
	}
};


// -----------------------------------------------------------------
// main test
// -----------------------------------------------------------------
int main()
{
	Server ss = Server(2000, 5);
	ss.startUp();
	return 0;
}
