#include "event.h"
#include "log.h"
#include "evutil.h"
#include "queue.h"

extern const struct eventop epollops;
static struct eventop* eventops[] = {
	(struct eventop*)&epollops//Ϊ�β�ǿ��ת������warning��
};


// ---------
// ȫ�ֱ���
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

	//˳��ŵ�base->evsel->init֮�󣬵��´���
	base->sig.ev_signal_pair[0] = -1;
	base->sig.ev_signal_pair[1] = -1;
	//��ʼ���¼�����
	base->evbase = NULL;
	base->evsel = eventops[0];
	base->evbase = base->evsel->init(base);

	if (base->evbase == NULL) {
		event_errx(1, "%s: no event mechanism available", __func__);    //����ӡerrno������Ϣ
	}

	//3��queue�ĳ�ʼ��
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
		return -1;    //��ʾ�Ѿ���������
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
// free�����е�heap
// -------------------
void event_base_free(struct event_base* base)
{
}

void event_queue_insert(struct event_base* base, struct event* ev, int queue)
{
	if (ev->ev_flags & queue) {
		if (queue & EVLIST_ACTIVE) { //������ظ�����active���У�ֱ�ӷ���
			return;
		}

		//��������������У����˳�
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
	//����û������ev_timeout��Ա��ֵ
	ev->ev_base = current_base;
	ev->ev_callback = callback;
	ev->ev_arg = arg;
	ev->ev_fd = fd;
	ev->ev_events = events;
	ev->ev_res = 0;//event������ԭ��
	ev->ev_flags = EVLIST_INIT;

	ev->ev_ncalls = 0;
	ev->ev_pncalls = NULL;

	if (current_base != NULL) {
		ev->ev_pri = current_base->nactive_queues/2;
	}
}

// -------------------------------------------------------------
// ��event_add�����tv��ֵ������ev->ev_timeout
// һ��event���ܼ��������У���EVLIST_INSERTED, EVLIST_TIMEOUT
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
		 * to do: min_heap�Ĳ���
		 */
	}

	//����Ҫ����EV_READ | EV_WRITE��ʱ�����EVLLIST_INSERTED����
	//���������timeout����Ҫ����timeout���У�һ����ʱ����Ҫ��2�������ж�ɾ������Ҫ��epoll��ɾ��
	if ( (ev->ev_events & (EV_READ|EV_WRITE|EV_SIGNAL)) &&
	     !(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE)) ) {//??????
		res = evsel->add(evbase, ev);//���뵽epoll

		if (res != -1) {
			event_queue_insert(base, ev, EVLIST_INSERTED);
		}
	}

	if (res != -1 && tv != NULL ) {
		struct ev_timeval now;

		//���Ѿ���timeout������  ?????
		if (ev->ev_flags & EVLIST_TIMEOUT) {
			event_queue_remove(base, ev, EVLIST_TIMEOUT);
		}

		//���Ѿ�����timeout������active������ ??????
		if ((ev->ev_flags & EVLIST_ACTIVE) && ev->ev_res & EV_TIMEOUT) {
			if (ev->ev_ncalls && ev->ev_pncalls) {//����ִ�лص�������������
				*ev->ev_pncalls = 0;
			}

			event_queue_remove(base, ev, EVLIST_ACTIVE);
		}

		get_time(base, &now);
		//evutil_timeradd(&now, tv, &ev->ev_timeout);
		//ev->ev_timeout = *tv;//��ʱ��ֵ
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

	//��event_process_active�У������event_del
	//ev_pncalls!=NULL��ʾ���ڽ��е���callback�Ĳ���
	//*ev->pncalls = 0��ev->ncall = 0������ִ��callback��whileѭ��
	if (ev->ev_ncalls && ev->ev_pncalls) {
		*ev->ev_pncalls = 0;
	}

	//��3�������У��ֱ�ɾ��
	if (ev->ev_flags & EVLIST_TIMEOUT) {
		event_queue_remove(base, ev, EVLIST_TIMEOUT);
	}

	if (ev->ev_flags & EVLIST_ACTIVE) {
		event_queue_remove(base, ev, EVLIST_ACTIVE);
	}

	//EVLIST_INSERT��ʾ��epoll�е�fd
	//eventһ�㶼����EVLIST_INSERTED��
	if (ev->ev_flags & EVLIST_INSERTED) {
		event_queue_remove(base, ev, EVLIST_INSERTED);
		return evsel->del(evbase, ev);//��epoll��ɾ��
	}

	return 0;
}

void event_active(struct event* ev, int res, short ncalls)
{
	if (ev->ev_flags & EVLIST_ACTIVE) {
		event_debugx("EVLIST_ACTIVE");
		ev->ev_res |= res;//����һ�ִ�����ԭ��
		return;
	}

	ev->ev_res = res;//����ΪEV_READ/EV_WRITE����event��α�����active�����е�
	ev->ev_ncalls = ncalls;//���ô���
	ev->ev_pncalls = NULL;//��ʾ��û�н���ִ��callback
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

	struct ev_timeval tv;// = {0, 0};//������ʱ��Ƭ����
	struct ev_timeval* tv_p;
	//struct ev_timeval tv_p, tv_1, tv_2;//epoll��ʱ����

	base->tv_cache.tv_sec = 0;//

	//ev_signal�Ѿ����ˣ������ù�signal_add
	if (base->sig.ev_signal_added) {
		evsignal_base = base;
	}

	while (1) {
		timeout_correct(base, &tv);//����event��ʱ��
		tv_p = &tv;
		timeout_next(base, tv_p);//�������ڿ���epoll��ʱ��ʱ��

		//base->event_tv, base->tv_cache֮���ʱ������dispatch�ĺ�ʱ
		get_time(base, &base->event_tv);//��֮֡ǰ
		base->tv_cache.tv_sec = 0;
		evsel->dispatch(base, evbase, tv_p);//����select
		get_time(base, &base->tv_cache);//��֮֡��

		timeout_process(base);//��timeout��event����active����

		if (base->event_count_active) {
			event_process_active(base);    //����active����
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

	//��TAILQ_FIRSTʼ�յõ�����ͷ������һ��ɾ��һ��
	for (ev=TAILQ_FIRST(activeq); ev!=NULL; ev=TAILQ_FIRST(activeq)) {
		if (ev->ev_events & EV_PERSIST) {
			event_queue_remove(base, ev, EVLIST_ACTIVE);    //������queue���Ƴ�
		} else { //����bufferevent_new��event
			event_del(ev);    //��������queue��ɾ��������epoll��ɾ��
		}

		ncalls = ev->ev_ncalls;
		ev->ev_pncalls = &ncalls;//ev_pcalls != NULL,��ʾ����active

		while (ncalls) {//��ε���
			ncalls--;
			ev->ev_ncalls = ncalls;
			(*ev->ev_callback)((int)ev->ev_fd, ev->ev_res, ev->ev_arg);//call�ص�����
		}
	}
}

void timeout_next(struct event_base* base, struct ev_timeval* tv)
{}
