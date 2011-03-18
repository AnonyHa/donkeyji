//#include <event-config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <string>
using namespace std;

void ClientReadCB(struct bufferevent* bufev, void* arg)
{
	printf("==========\n");
}

void ClientErrorCB(struct bufferevent* bufev, short flag, void* arg)
{
	printf("++++++++++\n");
}

static void fifo_read(int fd, short event, void *arg)
{
	printf("-----------\n");
	struct sockaddr_in addr;
	int len = sizeof(addr);
	bzero(&addr, len);
	char IpAddress[20];
	bzero(IpAddress, sizeof(IpAddress));

	int UserFd = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);

	if ( UserFd < 0 ) {
		return;
	}

	int flags = fcntl(UserFd, F_GETFL, 0);
	flags |= O_NONBLOCK;// 设置为非阻塞
	fcntl(UserFd, F_SETFL, flags);

	struct bufferevent* bev = bufferevent_new(UserFd, ClientReadCB, NULL, ClientErrorCB, 0);
	bufferevent_enable(bev, EV_READ|EV_WRITE);
	printf("-----end---\n");
	return;
}

int main (int argc, char **argv)
{
	int listen_socket, iFlags;
	struct sockaddr_in addr;
	struct event ev;

	listen_socket = socket(AF_INET, SOCK_STREAM, 0);// IPv4域-----

	if (listen_socket < 0) {
		return -1;
	}

	iFlags = fcntl(listen_socket, F_GETFL, 0);

	if (iFlags == -1 || fcntl(listen_socket, F_SETFL, iFlags | O_NONBLOCK)) {
		return -1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(1300);
	int optval = 1;

	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,//地址重用---
	               &optval, sizeof(optval)) == -1) {
		return -1;
	}

	if (bind(listen_socket, (struct sockaddr * ) &addr, sizeof(addr))<0) {
		return -1;
	}

	if (listen(listen_socket, 5) < 0) {
		return -1;
	}

	event_init();
	event_set(&ev, listen_socket, EV_READ|EV_PERSIST, fifo_read, NULL);
	event_add(&ev, NULL);

	event_dispatch();
	return (0);
}
