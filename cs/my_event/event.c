#include "event.h"
#include "log.h"
#include "evutil.h"
#include "queue.h"

extern const struct eventop epollops;
static struct eventop* eventops[] = {
	(struct eventop*)&epollops//为何不强行转换总是warning呢
};


// ---------
// 全局变量
// ---------
struct event_base* current_base = NULL;
extern struct event_base* evsignal_base;

// ------------------
// tv_3 = tv_1 - tv_2
// ------------------
void timeval_sub(struct ev_timeval* tv_1, struct ev_timeval* tv_2, struct ev_timeval* tv_3)
{}

void get_time(struct event_base* base, struct ev_timeval* tv)
{}

// --------------------------
// 1: tv_1>tv_2  0: ==  -1: <
// --------------------------
int timeval_compare(struct ev_timeval* tv_1, struct ev_timeval* tv_2)
{}

struct event_base* event_init() {
	struct event_base* base = event_base_new();

	if (base != NULL) {
		current_base = base;
	}

	return base;
}

struct event_base* event_base_new() {
	struct event_base* base = calloc(1, sizeof(struct event_base));

	if (base == NULL) {
		event_err(1, "%s: calloc", __func__);    //---
	}

	get_time(base, &base->event_tv);

	//顺序放到base->evsel->init之后，导致错误
	base->sig.ev_signal_pair[0] = -1;
	base->sig.ev_signal_pair[1] = -1;
	//初始化事件引擎
	base->evbase = NULL;
	base->evsel = eventops[0];
	base->evbase = base->evsel->init(base);

	if (base->evbase == NULL) {
		event_errx(1, "%s: no event mechanism available", __func__);    //不打印errno错误消息
	}

	//3个queue的初始化
	TAILQ_INIT(&base->inserted_queue);
	//TAILQ_INIT(&base->active_queue);
	event_base_priority_init(base, 1);
	TAILQ_INIT(&base->timeout_queue);

	//?????
	event_debugx("###########");

	return base;
}

int event_base_priority_init(struct event_base* base, int npriorities)
{
	int i;

	if (base->event_count_active != 0) {
		return -1;    //表示已经建立过了
	}

	if (base->nactive_queues != 0 && npriorities != base->nactive_queues) {
		for (i=0; i<base->nactive_queues; i++) {
			free(base->active_queue[i]);
		}

		free(base->active_queue);
	}

	base->nactive_queues = npriorities;
	base->active_queue = (struct event_list**)calloc(base->nactive_queues, npriorities * sizeof(struct event_list*));

	if (base->active_queue == NULL) {
		event_err(1, "%s: calloc", __func__);
	}

	for (i=0; i<base->nactive_queues; i++) {
		base->active_queue[i] = malloc(sizeof(struct event_list));

		if (base->active_queue[i] == NULL) {
			event_err(1, "%s: malloce", __func__);
		}

		TAILQ_INIT(base->active_queue[i]);
	}
}

// -------------------
// free掉所有的heap
// -------------------
void event_base_free(struct event_base* base)
{
}

void event_queue_insert(struct event_base* base, struct event* ev, int queue)
{
	if (ev->ev_flags & queue) {
		if (queue & EVLIST_ACTIVE) { //如果是重复进入active队列，直接返回
			return;
		}

		//重入进入其他队列，则退出
		event_errx(1, "%s: %p(fd %d) already on queue %x", __func__, ev, ev->ev_fd, queue);
	}

	base->event_count += 1;
	ev->ev_flags |= queue;

	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_INSERT_TAIL(&base->inserted_queue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		base->event_count_active += 1;
		TAILQ_INSERT_TAIL(base->active_queue[ev->ev_pri], ev, ev_active_next);
		break;
	case EVLIST_TIMEOUT:
		TAILQ_INSERT_TAIL(&base->timeout_queue, ev, ev_timeout_next);
		break;
	default:
		event_errx(1, "%s: unknown queue %x", __func__, queue);
		break;
	}
}

void event_queue_remove(struct event_base* base, struct event* ev, int queue)
{
	if (!(ev->ev_flags & queue)) {
		event_errx(1, "%s: %p(fd %d) not on queue %x", __func__, ev, ev->ev_fd, queue);
	}

	base->event_count -= 1;
	ev->ev_flags &= ~queue;

	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_REMOVE(&base->inserted_queue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		TAILQ_REMOVE(base->active_queue[ev->ev_pri], ev, ev_active_next);
		base->event_count_active -= 1;
		break;
	case EVLIST_TIMEOUT:
		TAILQ_REMOVE(&base->timeout_queue, ev, ev_timeout_next);
		break;
	default:
		event_errx(1, "%s: unkown queue %x", __func__, queue);
		break;
	}
}

void event_set(struct event* ev, int fd, short events, void (*callback)(int, short, void*), void* arg)
{
	//这里没有设置ev_timeout成员的值
	ev->ev_base = current_base;
	ev->ev_callback = callback;
	ev->ev_arg = arg;
	ev->ev_fd = fd;
	ev->ev_events = events;
	ev->ev_res = 0;//event触发的原因
	ev->ev_flags = EVLIST_INIT;

	ev->ev_ncalls = 0;
	ev->ev_pncalls = NULL;

	if (current_base != NULL) {
		ev->ev_pri = current_base->nactive_queues/2;
	}
}

// -------------------------------------------------------------
// 在event_add里根据tv的值来设置ev->ev_timeout
// 一个event可能加入多个队列，如EVLIST_INSERTED, EVLIST_TIMEOUT
// -------------------------------------------------------------
int event_add(struct event* ev, const struct ev_timeval* tv)
{
	struct event_base* base = ev->ev_base;
	const struct eventop* evsel = base->evsel;
	void* evbase = base->evbase;
	int res = 0;

	event_debugx(
	    "event_add: event: %p, %s%s%scall %p",
	    ev,
	    ev->ev_events & EV_READ ? "EV_READ" : " ",
	    ev->ev_events & EV_WRITE ? "EV_WRITE" : " ",
	    tv ? "EV_TIMEOUT" : " ",
	    ev->ev_callback
	);

	if (tv != NULL && !(ev->ev_flags & EVLIST_TIMEOUT)) {
		/*
		 * to do: min_heap的操作
		 */
	}

	//当需要监听EV_READ | EV_WRITE的时候加入EVLLIST_INSERTED队列
	//如果设置了timeout，还要加入timeout队列，一旦超时，需要从2个队列中都删除，且要从epoll中删除
	if ( (ev->ev_events & (EV_READ|EV_WRITE|EV_SIGNAL)) &&
	     !(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE)) ) {//??????
		res = evsel->add(evbase, ev);//加入到epoll

		if (res != -1) {
			event_queue_insert(base, ev, EVLIST_INSERTED);
		}
	}

	if (res != -1 && tv != NULL ) {
		struct ev_timeval now;

		//若已经在timeout队列中  ?????
		if (ev->ev_flags & EVLIST_TIMEOUT) {
			event_queue_remove(base, ev, EVLIST_TIMEOUT);
		}

		//若已经由于timeout放入了active队列中 ??????
		if ((ev->ev_flags & EVLIST_ACTIVE) && ev->ev_res & EV_TIMEOUT) {
			if (ev->ev_ncalls && ev->ev_pncalls) {//正在执行回调函数，则跳出
				*ev->ev_pncalls = 0;
			}

			event_queue_remove(base, ev, EVLIST_ACTIVE);
		}

		get_time(base, &now);
		//evutil_timeradd(&now, tv, &ev->ev_timeout);
		//ev->ev_timeout = *tv;//暂时赋值
		event_debugx("timeout event add");
		event_queue_insert(base, ev, EVLIST_TIMEOUT);
	}

	return res;
}

int event_del(struct event* ev)
{
	struct event_base* base;// = ev->ev_base;
	struct eventop* evsel;// = base->evsel;
	void* evbase;// = base->evbase;

	//event_debugx("event_del: %p, callback %p", ev, ev->ev_callback);

	if (ev->ev_base == NULL) {
		return -1;
	}

	base = ev->ev_base;
	evsel = base->evsel;
	evbase = base->evbase;

	//在event_process_active中，会调用event_del
	//ev_pncalls!=NULL表示正在进行调用callback的操作
	//*ev->pncalls = 0将ev->ncall = 0，跳出执行callback的while循环
	if (ev->ev_ncalls && ev->ev_pncalls) {
		*ev->ev_pncalls = 0;
	}

	//在3个容器中，分别删除
	if (ev->ev_flags & EVLIST_TIMEOUT) {
		event_queue_remove(base, ev, EVLIST_TIMEOUT);
	}

	if (ev->ev_flags & EVLIST_ACTIVE) {
		event_queue_remove(base, ev, EVLIST_ACTIVE);
	}

	//EVLIST_INSERT表示在epoll中的fd
	//event一般都会在EVLIST_INSERTED中
	if (ev->ev_flags & EVLIST_INSERTED) {
		event_queue_remove(base, ev, EVLIST_INSERTED);
		return evsel->del(evbase, ev);//从epoll中删除
	}

	return 0;
}

void event_active(struct event* ev, int res, short ncalls)
{
	if (ev->ev_flags & EVLIST_ACTIVE) {
		event_debugx("EVLIST_ACTIVE");
		ev->ev_res |= res;//增加一种触发的原因
		return;
	}

	ev->ev_res = res;//设置为EV_READ/EV_WRITE，该event因何被放入active队列中的
	ev->ev_ncalls = ncalls;//调用次数
	ev->ev_pncalls = NULL;//表示还没有进行执行callback
	event_queue_insert(ev->ev_base, ev, EVLIST_ACTIVE);
}

//-------------------------------------------------------------------
int event_dispatch()
{
	return event_loop(0);
}

int event_loop(flags)
{
	return event_base_loop(current_base, flags);
}

int event_base_loop(struct event_base* base, int flags)
{
	struct eventop* evsel = base->evsel;
	void* evbase = base->evbase;

	struct ev_timeval tv;// = {0, 0};//经过的时间片长度
	struct ev_timeval* tv_p;
	//struct ev_timeval tv_p, tv_1, tv_2;//epoll超时控制

	base->tv_cache.tv_sec = 0;//

	//ev_signal已经加了，即调用过signal_add
	if (base->sig.ev_signal_added) {
		evsignal_base = base;
	}

	while (1) {
		timeout_correct(base, &tv);//更新event的时间
		tv_p = &tv;
		timeout_next(base, tv_p);//计算用于控制epoll超时的时间

		//base->event_tv, base->tv_cache之间的时间差就是dispatch的耗时
		get_time(base, &base->event_tv);//该帧之前
		base->tv_cache.tv_sec = 0;
		evsel->dispatch(base, evbase, tv_p);//处理select
		get_time(base, &base->tv_cache);//该帧之后

		timeout_process(base);//将timeout的event放入active队列

		if (base->event_count_active) {
			event_process_active(base);    //处理active队列
		}
	}
}

void timeout_correct(struct event_base* base, struct ev_timeval* tv)
{
}

void timeout_process(struct event_base* base)
{
}

void event_process_active(struct event_base* base)
{
	struct event* ev;
	struct event_list* activeq = NULL;
	int i;
	short ncalls;

	for (i=0; i<base->nactive_queues; i++) {
		if (TAILQ_FIRST(base->active_queue[i]) != NULL) {
			activeq = base->active_queue[i];
			break;
		}
	}

	//用TAILQ_FIRST始终得到链表头，访问一个删除一个
	for (ev=TAILQ_FIRST(activeq); ev!=NULL; ev=TAILQ_FIRST(activeq)) {
		if (ev->ev_events & EV_PERSIST) {
			event_queue_remove(base, ev, EVLIST_ACTIVE);    //仅仅从queue中移除
		} else { //对于bufferevent_new的event
			event_del(ev);    //不仅仅从queue中删除，还从epoll中删除
		}

		ncalls = ev->ev_ncalls;
		ev->ev_pncalls = &ncalls;//ev_pcalls != NULL,表示正在active

		while (ncalls) {//多次调用
			ncalls--;
			ev->ev_ncalls = ncalls;
			(*ev->ev_callback)((int)ev->ev_fd, ev->ev_res, ev->ev_arg);//call回调函数
		}
	}
}

void timeout_next(struct event_base* base, struct ev_timeval* tv)
{}
