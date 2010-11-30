#include "event.h"
#include "evutil.h"

void client_readcb(struct bufferevent* bufev, void* arg)
{
	event_debugx("client_readcb");
}

void client_writecb(struct bufferevent* bufev, void* arg)
{
	event_debugx("client_writecb");
}

void client_errorcb(struct bufferevent* bufev, short what, void* arg)
{
	event_debugx("client_errorcb  what = %d", what);

	int len = bufev->output->off;
	//event_debugx("left len = %d", len);
	if (len > 0) {//若len为0，调用send会导致错误
		if (evbuffer_write(bufev->output, bufev->ev_write.ev_fd) != len) {
			event_err(1, "lost data");
		}
	}
	int fd = bufev->ev_write.ev_fd; 
	//event_debugx("close fd = %d", fd);
	bufferevent_free(bufev);//删除event，从epoll中删除 fd
	close(fd);//将没有发送的发送出去
	bufev = NULL;
}

void new_client(int fd, short event, void* arg)
{
	struct sockaddr_in addr;
	int len = sizeof(addr);
	bzero(&addr, len);
	char ip_address[20];
	bzero(ip_address, sizeof(ip_address));

	int user_fd = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&len);
	if ( user_fd < 0 ) return;
	event_debugx("new_client: user_fd = %d", user_fd);

	int flags = fcntl(user_fd, F_GETFL, 0);
	flags |= O_NONBLOCK;// 设置为非阻塞
	fcntl(user_fd, F_SETFL, flags);

	struct bufferevent* bufev = bufferevent_new(user_fd, client_readcb, NULL, client_errorcb, NULL);
	bufferevent_enable(bufev, EV_READ|EV_WRITE);
	//event_debugx("-----------");

	//*  为何一关闭客户端，会导致服务器关掉????
	//event_debugx("before bufferevent_write ");
	//int n = bufferevent_write(bufev, "hello", 5);
	//event_debugx("after bufferevent_write res = %d", n);
	//*/
}

void sig_cb(int fd, short event, void* arg)
{
	event_debugx("signal callback");
}

void timer_cb(int fd, short events, void* arg)
{
	event_debugx("timer callback");
}

int set_noblocking(int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		return -1;
	}
	return 0;
}

int create_listen_sock()
{
	int listen_sock;
	listen_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_sock == -1) return -1;
	if (set_noblocking(listen_sock) == -1) return -1;
	int opt = 1;//非零表示启用该选项, 0 表示不启用
	unsigned int size = sizeof(opt);
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, size);

	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_port = htons(1300);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htons(INADDR_ANY);
	if ( bind(listen_sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1 ) return -1;

	if (listen(listen_sock, 5) == -1) {
		perror("listen");
		return -1;
	}
	return listen_sock;
}

int main()
{
	int listen_sock;
	struct event listen_event;
	struct event sig_event;
	struct event timer_event;
	struct ev_timeval tv;

	listen_sock = create_listen_sock();
	if (listen_sock == -1) event_err(1, "fail to create listen sock");

	event_init();

	//socket类型 event
	event_set(&listen_event, listen_sock, EV_READ|EV_PERSIST, new_client, NULL);
	event_add(&listen_event, NULL);	

	//信号event
	signal_set(&sig_event, SIGINT, sig_cb, &sig_event);
	signal_add(&sig_event, NULL);

	//timer event
	evutil_timerclear(&tv);
	tv.tv_sec = 2;
	evtimer_set(&timer_event, timer_cb, &timer_event);
	evtimer_add(&timer_event, &tv);

	event_dispatch();
	event_del(&listen_event);
	close(listen_sock);
	event_debugx("end main");

	return 0;
}
