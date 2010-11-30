#include "op_epoll.h"
#include <stdio.h> 
#include <iostream>
#include <errno.h>

EpollOp::EpollOp()
{
	_rt.rlim_max = _rt.rlim_cur = MAXEPOLLSIZE;
	_epfd = epoll_create(MAXEPOLLSIZE);
	_fdCnt = 0;
	_events = new epoll_event[MAXEPOLLSIZE];
}

EpollOp::~EpollOp()
{
	delete[] _events;
}

int EpollOp::addFd(int fd, int event)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	if (event == EV_READ)
		ev.events = EPOLLIN | EPOLLET;
	else if (event == EV_WRITE)
		ev.events = EPOLLOUT | EPOLLET;
	else if (event == EV_RW)
		ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
	else// 其他的不处理
		return -1;
	int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
	if (ret == -1) {
		perror("epoll_ctl");
		return -1;
	} else if (ret == 0) {
		_fdCnt++;
		return 0;
	}
}

int EpollOp::rmFd(int fd, void* ev)
{
	int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, (struct epoll_event*)ev);
	if (ret == -1) {
		std::cout<<"errno = "<<errno<<std::endl;
		perror("epoll_ctl");
		return -1;
	} else if (ret == 0) {
		_fdCnt--;
		return 0;
	}
}

int EpollOp::doDispatch()
{
	std::cout<<"_fdCnt = "<<_fdCnt<<std::endl;
	int ret = epoll_wait(_epfd, _events, _fdCnt, -1);
	return ret;
}
