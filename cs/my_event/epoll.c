#include "event.h"

//记录与fd对应的读写event，初始值与fd的event一样
struct evepoll {
	struct event* evread;
	struct event* evwrite;
};

//记录epoll相关的数据
struct epollop {
	struct evepoll* fds;//为每一个fd记录2个event，作为内存池使用
	int nfds;
	struct epoll_event* events;//epoll专用
	int nevents;
	int epfd;
};

static void* epoll_init(struct event_base* base);
static int epoll_add(void* arg, struct event* ev);
static int epoll_del(void* arg, struct event* ev);
static int epoll_dispatch(struct event_base* base, void* arg, struct ev_timeval* tv);

// --------------------
// event_base.evsel的值
// --------------------
const struct eventop epollops = {
	epoll_init,
	epoll_add,
	epoll_del,
	epoll_dispatch
};


#define MAX_EPOLL_TIMEOUT_MSEC (35*60*1000)
// -----------------------
//上层根据返回值来错误处理
// -----------------------
// 相当于struct epollop的构造函数
void* epoll_init(struct event_base* base)//epoll_create
{
	int epfd;
	int nfiles = NEVENT;
	struct rlimit rl;
	struct epollop* eop;

	if (getrlimit(RLIMIT_NOFILE, &rl)==0 && rl.rlim_cur!=RLIM_INFINITY) {
		nfiles = rl.rlim_cur - 1;
	}

	epfd = epoll_create(nfiles);

	if (epfd == -1) {
		if (errno != ENOSYS) {
			event_warn("epoll_create fail");
		}

		return NULL;
	}

	eop = (struct epollop*)calloc(1, sizeof(struct epollop));

	if (eop == NULL) {
		return NULL;
	}

	eop->epfd = epfd;
	eop->events = malloc(nfiles * sizeof(struct epoll_event));

	if (eop->events == NULL) {
		free(eop);
		return NULL;
	}

	eop->nevents = nfiles;

	// 此处相当于内存池，预先申请了nfiles个 event* 的空间，专门用来处理读写的event
	eop->fds = calloc(nfiles, sizeof(struct evepoll));//2个与fd关联的read/write事件

	if (eop->fds == NULL) {
		free(eop->events);
		free(eop);
		return NULL;
	}

	eop->nfds = nfiles;

	//信号的处理
	evsignal_init(base);

	//返回给current_base，记录在event_base.evbase指针上
	return eop;//to do: 注意要free掉eop的内存
}

// ----------------------------------------------------------------------
// arg: event_base.evbase
// struct epoll_data {vodi *ptr; int fd; __unit32_t u32; __unit64_t u64;}
// struct epoll_event {__unit32_t events; epoll_data_t data;}
// ----------------------------------------------------------------------
// void* arg：struct epollop指针，相当于epollop的成员函数，面向对象
int epoll_add(void* arg, struct event* ev)//添加到epoll中去, epoll_ctl
{
	struct epollop* eop = arg;
	struct epoll_event epev = {0, {0}};
	struct evepoll* evep;
	int op;
	int fd;
	int events;

	/*
	 * to do: 处理信号的情况
	 */
	if (ev->ev_events & EV_SIGNAL) {
		return evsignal_add(ev);
	}

	fd = ev->ev_fd;

	if (fd >= eop->nfds) {
		/*
		 * to do:
		 */
		return -1;
	}//超过最大的连接数

	evep = &eop->fds[fd];//与fd对应的2个event
	op = EPOLL_CTL_ADD;
	events = 0;

	//evepoll->evread, evepoll->evwrite初始化为NULL
	//一旦evep->wvread不为NULL就表示对fd已经调用过epoll_ctl了
	if (evep->evread != NULL) {
		events |= EPOLLIN;
		op = EPOLL_CTL_MOD;//已经添加过了
	}

	if (evep->evwrite != NULL) {
		events |= EPOLLOUT;
		op = EPOLL_CTL_MOD;//已经添加过了
	}

	//event_debugx("op = %s", op==EPOLL_CTL_MOD?"EPOLL_CTL_MOD":"EPOLL_CTL_ADD");

	//对于bufferevent_new, bufferevent_enable，对同一个fd会调用2次epoll_ctl
	if (ev->ev_events & EV_READ) {
		events |= EPOLLIN;    //用 |= 来增加监视类型
	}

	if (ev->ev_events & EV_WRITE) {
		events |= EPOLLOUT;
	}

	epev.data.fd = fd;//多余???
	epev.data.ptr = evep;
	epev.events = events;
	epoll_ctl(eop->epfd, op, ev->ev_fd, &epev);
	event_debugx("epoll_add: fd = %d", ev->ev_fd);
	//event_debugx("eop: %d", eop->epfd);

	//epollop->fds中只保存与fd对应的2个event的指针，对于listen_sock，只有一个event是有效的;对与connect_sock，2个event都是有效的
	//对于listen_sock，只监视EV_READ
	//对于connect_sock, 用2个event分别监视EV_READ,EV_WRITE
	//不会有一个event同时监视EV_READ和EV_WRITE,避免了混乱
	if (ev->ev_events & EV_READ) {
		evep->evread = ev;
	}

	if (ev->ev_events & EV_WRITE) {
		evep->evwrite = ev;
	}

	return 0;
}

//对于bufferevent的fd，会调用2次epoll_del
//第一次op = EPOLL_CTL_MOD
//第二次op = EPOLL_CTL_DEL
int epoll_del (void* arg, struct event* ev)//从epoll中删去, epoll_ctl
{
	event_debugx("epoll_del");
	struct epollop* eop = arg;
	struct epoll_event epev = {0, {0}};
	struct evepoll* evep;
	int fd;
	int op;
	int events;
	int needwritedelete = 1;
	int needreaddelete = 1;

	//process signal
	if (ev->ev_events & EV_SIGNAL) {
		return evsignal_del(ev);
	}

	fd = ev->ev_fd;

	if (fd >= eop->nfds) {
		return 0;
	}

	evep = &eop->fds[fd];

	op = EPOLL_CTL_DEL;
	events = 0;

	if (ev->ev_events & EV_READ) {
		events |= EPOLLIN;    //删除的ev是监视EPOLLIN的
	}

	if (ev->ev_events & EV_WRITE) {
		events |= EPOLLOUT;
	}

	//若此时events & (EPOLLIN|EPOLLOUT) == (EPOLLIN|EPOLLOUT) ，表示此event同时监视EV_READ, EV_WRITE
	//这种情况下直接op = EPOLL_CTL_DEL
	if ((events & (EPOLLIN|EPOLLOUT)) != (EPOLLIN|EPOLLOUT)) {//events不同时包含EPOLLIN, EPOLLOUT
		//下面这个if/else if在2次执行时，只会第一次执行其中一个
		if ((events & EPOLLIN) && (evep->evwrite != NULL)) {//保留evep->evwrite
			needwritedelete = 0;
			events = EPOLLOUT;
			op = EPOLL_CTL_MOD;
		} else if ((events & EPOLLOUT) && evep->evread != NULL) {//保留evep->evread
			needreaddelete = 0;
			events = EPOLLIN;
			op = EPOLL_CTL_MOD;
		}
	}

	//event_debugx("op = %s", op==EPOLL_CTL_MOD?"EPOLL_CTL_MOD":"EPOLL_CTL_DEL");

	epev.events = events;
	epev.data.ptr = evep;

	//更新保存的fd对应的event指针
	if (needreaddelete) {
		evep->evread = NULL;
	}

	if (needwritedelete) {
		evep->evwrite = NULL;
	}

	//if (epoll_ctl(eop->epfd, op, fd, &epev) == -1) return -1;
	if (epoll_ctl(eop->epfd, op, fd, NULL) == -1) {
		return -1;
	}

	return 0;
}

// ----------------------
// arg: event_base.evbase
// ----------------------
int epoll_dispatch(struct event_base* base, void* arg, struct ev_timeval* tv)
{
	struct epollop* eop = arg;
	struct epoll_event* events = eop->events;//epoll_event对象池
	struct evepoll* evep;
	int res;
	int i;
	int timeout = -1;

	if (tv != NULL) {
		timeout = tv->tv_sec * 1000 + (tv->tv_usec + 999)/1000;
	}

	//event_debugx("timeout = %d", timeout);
	if (timeout > MAX_EPOLL_TIMEOUT_MSEC) {
		timeout = MAX_EPOLL_TIMEOUT_MSEC;
	}

	res = epoll_wait(eop->epfd, events, eop->nevents, timeout);

	if (res == -1) {
		if (errno != EINTR) {
			event_warn("epoll_wait");
			return -1;
		}

		//event_debugx("1111111");
		evsignal_process(base);
		return 0;
	} else if (base->sig.evsignal_caught) {//调用成功，但是有信号发生
		//event_debugx("2222222");
		evsignal_process(base);
	}

	//event_debugx("%s: epoll_wait reports %d", "__func__", res);

	for (i=0; i<res; i++) {
		int what = events[i].events;//发生的是何种事件
		struct event* evread = NULL;
		struct event* evwrite = NULL;

		evep = events[i].data.ptr;//该fd对应的read/write event，已经建立的event对象池

		if (what & (EPOLLHUP|EPOLLERR)) {//对方发生错误的时候，即断开的
			event_debugx("EPOLLHUP | EPOLLERR");
			evread = evep->evread;
			evwrite = evep->evwrite;
		} else {
			if (what & EPOLLIN) {
				event_debugx("EPOLLIN");
				evread = evep->evread;
			}

			if (what & EPOLLOUT) {//如何触发的???
				//event_debugx("EPOLLOUT");
				evwrite = evep->evwrite;
			}
		}

		if (!(evread || evwrite)) {
			continue;
		}

		//fd对应的read/write事件放入active队列
		//evread->ev_res = EV_READ,表示event上发生的EV_READ事件
		//evread与ev相同
		if (evread != NULL) {
			event_debugx("read come");
			event_active(evread, EV_READ, 1);//因为EV_READ而被触发
		}

		if (evwrite != NULL) {
			//event_debugx("write come");
			event_active(evwrite, EV_WRITE, 1);
		}
	}

	return 0;
}
