#ifndef _EVENT_H
#define _EVENT_H

#ifdef __cplusplus
extern "C" {
#endif


//linux
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <fcntl.h>

//ansi c
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


typedef unsigned char u_char;
typedef unsigned short u_short;

//macro
#define NEVENT 20000 


//event所处队列的类型
#define EVLIST_TIMEOUT 	0x01
#define EVLIST_INSERTED 0x02//在epoll中的fd
#define EVLIST_SIGNAL	0x04
#define EVLIST_ACTIVE 	0x08
#define EVLIST_INTERNAL	0x10
#define EVLIST_INIT 	0x80//不在任何队列中

//event的事件类型
#define EV_TIMEOUT 	0x01
#define EV_READ 	0x02
#define EV_WRITE 	0x04
#define EV_SIGNAL	0x08
#define EV_PERSIST	0x10

//log级别
#define _EVENT_LOG_DEBUG 	0
#define _EVENT_LOG_MSG 		1
#define _EVENT_LOG_WARN 	2
#define _EVENT_LOG_ERR 		3
typedef void (*event_log_cb)(int log_level, const char* msg);

//时间描述
struct ev_timeval
{
	long tv_sec;
	long tv_usec;
};

//除了在main函数栈中的event对象，其余由libevent产生的event对象都在heap中，由libevent统一管理
#define TAILQ_ENTRY(type)	\
struct {	\
	struct type* tqe_next;	\
	struct type** tqe_prev;	\
}

//前置申明
struct event_base;
struct event//每一个event在多个链表中
{
	TAILQ_ENTRY(event) ev_next;//inserted_queue里的下一个event
	TAILQ_ENTRY(event) ev_active_next;//for active_queue
	TAILQ_ENTRY(event) ev_timeout_next;//for timeout_queue
	TAILQ_ENTRY(event) ev_signal_next;//for signal_queue

	int ev_fd;//相关联的fd
	short ev_events;//事件类型
	struct ev_timeval ev_timeout;//超时时间
	void (*ev_callback) (int, short, void* arg);
	void* ev_arg;
	int ev_res;//event因何被放入active队列的原因
	struct event_base* ev_base;
	int ev_flags;//所处队列的类型
	short ev_ncalls;//被active后，调用callback的次数
	short* ev_pncalls;//ev_ncalls的指针
	int ev_pri;
};

#define EVENT_SIGNAL(ev)	(int)(ev)->ev_fd
#define EVENT_FD(ev)	(int)(ev)->ev_fd

//container of event
struct event_list
{
	struct event* tqh_first;//存放event指针
	struct event** tqh_last;
};


//------------------------------------
#include "evsignal.h"
//------------------------------------

//事件引擎
struct eventop
{
	void* (*init) (struct event_base*);//epoll_create
	int (*add) (void*, struct event*);//添加到epoll中去, epoll_ctl
	int (*del) (void*, struct event*);//从epoll中删去, epoll_ctl
	int (*dispatch) (struct event_base*, void*, struct ev_timeval*);//调用epoll_wait()，直接放入active队列
};

//全局定义
struct event_base
{
	struct eventop* evsel;
	void* evbase;

	int event_count;
	int event_count_active;

	struct event_list inserted_queue;//普通队列
	struct event_list** active_queue;//活动队列
	int nactive_queues;
	struct event_list timeout_queue;//超时队列

	struct ev_timeval tv_cache;
	struct ev_timeval event_tv;

	struct evsignal_info sig;
};


// ---------
// queue
// ---------
struct event_list* create_event_queue();
void del_event_queue(struct event_list* eq);
void insert_queue(struct event_list* eq, struct event* ev, int type);
void remove_queue(struct event_list* eq, struct event* ev);

// --------
// ev_timeval
// --------
void timeval_sub(struct ev_timeval* tv_1, struct ev_timeval* tv_2, struct ev_timeval* tv_3);

void get_time(struct event_base* base, struct ev_timeval* tv);

int timeval_compare(struct ev_timeval* tv_1, struct ev_timeval* tv_2);

// -----------
// event_base
// -----------
struct event_base* event_init();

struct event_base* event_base_new();

void event_base_free();

int event_base_priority_init(struct event_base* base, int npriorities);

// ------------
// event_list
// ------------
void event_queue_insert(struct event_base* base, struct event* ev, int queue);

void event_queue_remove(struct event_base* base, struct event* ev, int queue);

#define signal_add(ev, tv)	event_add(ev, tv)
#define signal_set(ev, x, cb, arg)	event_set(ev, x, EV_SIGNAL|EV_PERSIST, cb, arg)
#define signal_del(ev)	event_del(ev)

// ----------
// event
// ----------
void event_set(struct event* ev, int fd, short events, void (*callback)(int, short, void*), void* arg);

int event_add(struct event* ev, const struct ev_timeval* tv);

int event_del(struct event* ev);

void event_active(struct event* ev, int res, short ncalls);

int event_dispatch();

int event_loop(int flags);

int event_base_loop(struct event_base* base, int flags);

void timeout_correct(struct event_base* base, struct ev_timeval* tv);

void timeout_process(struct event_base* base);

void event_process_active(struct event_base* base);

void timeout_next(struct event_base* base, struct ev_timeval* tv);

// ---------------
// for bufferevent
// ---------------
// 数据加入缓冲区的末尾
// 从缓冲区的头部消耗数据
struct evbuffer
{
	u_char* buffer;//有效数据的地址
	u_char* orig_buffer;//整个缓冲区的地址

	size_t misalign;//buffer相对于orig_buffer的位移
	size_t totallen;//整个缓冲区的大小
	size_t off;//有效数据的长度

	void (*cb)(struct evbuffer*, size_t, size_t, void*);
	void* cbarg;
};

#define EVBUFFER_READ 		0x01
#define EVBUFFER_WRITE		0x02
#define EVBUFFER_EOF		0x10
#define EVBUFFER_ERROR		0x20
#define EVBUFFER_TIMEOUT	0x40

struct bufferevent;//前置申明
typedef void (*evbuffercb) (struct bufferevent*, void*);
typedef void (*everrorcb) (struct bufferevent*, short what, void*);

struct event_watermark
{
	size_t low;
	size_t high;
};

//包含2个event,关联同一个fd
struct bufferevent
{
	struct event_base* ev_base;

	struct event ev_read;
	struct event ev_write;

	struct evbuffer* input;
	struct evbuffer* output;

	struct event_watermark wm_read;
	struct event_watermark wm_write;

	evbuffercb readcb;
	evbuffercb writecb;
	everrorcb errorcb;
	void* cbarg;

	int timeout_read;
	int timeout_write;

	short enabled;
};

void bufferevent_setcb(struct bufferevent* bufev, evbuffercb readcb, evbuffercb writecb, everrorcb errorcb, void* cbarg);

struct bufferevent* bufferevent_new(int fd, evbuffercb readcb, evbuffercb writecb, everrorcb errorcb, void* cbarg);

void bufferevent_free(struct bufferevent* bufev);

int bufferevent_enable(struct bufferevent* bufev, short event);

void bufferevent_setwatermark(struct bufferevent* bufev, short events, size_t lowmark, size_t highmark);


int bufferevent_write(struct bufferevent* bufev, const void* data, size_t size);

int bufferevent_read(struct bufferevent* bufev, void* data, size_t size);

//evbuffer
#define EVBUFFER_LENGTH(x)	(x)->off
#define EVBUFFER_DATA(x)	(x)->buffer
#define EVBUFFER_INPUT(x)	(x)->input
#define EVBUFFER_OUTPUT(x)	(x)->output

struct evbuffer* evbuffer_new(void);

void evbuffer_free(struct evbuffer* buf);

int evbuffer_read(struct evbuffer* buf, int fd, int howmuch);

int evbuffer_write(struct evbuffer* buf, int fd);

int evbuffer_expand(struct evbuffer* buf, size_t datlen);

int evbuffer_add(struct evbuffer* buf, const void* data, size_t size);

//timer event
#define evtimer_set(ev, cb, arg)		event_set(ev, -1, 0, cb, arg)
#define evtimer_add(ev, tv)		event_add(ev, tv)

#define timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0

#ifdef __cplusplus
}
#endif

#endif
