#include <sys/epoll.h>
#include <sys/resource.h>

// ------------
// IO驱动基类
// ------------
class BaseOp
{
public:
	virtual int addFd(int fd, int event) = 0;
	virtual int rmFd(int fd, void* ev) = 0;
	virtual int doDispatch() = 0;
};

// ----------------
// epoll
// ----------------
const int MAXEPOLLSIZE = 3600;
const int EV_RW = 0;
const int EV_READ = 1;
const int EV_WRITE = 2;

class Server;
class EpollOp : public BaseOp
{
	friend class Server;
private:
	struct epoll_event* _events;//[MAXEPOLLSIZE];
	//struct rlimit _rt;
	int _epfd;
	int _fdCnt;
public:
	EpollOp();
	~EpollOp();

	virtual int addFd(int fd, int event);
	virtual int rmFd(int fd, void* ev);
	virtual int doDispatch(); 
};

class OpEpoll
{
private:
	struct epoll_event* _events;
	int _epfd;
	int _nfds;// fd 数量
public:
	OpEpoll();
	~OpEpoll();

	virtual int add(int fd, int event);
	virtual int del(int fd, int event);// 对应epoll，不用event这个参数
	virtual int dispatch();
};
