#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "config.h"
#include "timer.h"
#include "min_heap.h"
#include "list.h"
#include "util.h"

//--------------------
// module variable
//--------------------
static min_heap_t* mh = NULL;
static struct list* tl = NULL;
static struct m_list* ml = NULL;


#ifdef HAVE_SELECT
extern struct timer_model select_model;
#endif

#ifdef HAVE_SELECT
struct timer_model* tm = &select_model;
#endif

//----------------
//module init
//----------------
int timer_init()
{
	if (mh != NULL) {
		min_heap_dtor(mh);
	}

	mh = (min_heap_t*)malloc(sizeof(min_heap_t));

	if (mh == NULL) {
		goto error;
	}

	min_heap_ctor(mh);

	if (tl != NULL) {
		list_free(tl);
	}

	tl = list_new();

	if (tl == NULL) {
		goto error;
	}

	if (ml != NULL) {
		m_list_free(ml);
	}

	ml = m_list_new();

	if (ml == NULL) {
		goto error;
	}

	return 0;
error:

	if (mh != NULL) {
		min_heap_dtor(mh);
	}

	if (tl != NULL) {
		list_free(tl);
	}

	if (ml != NULL) {
		m_list_free(ml);
	}

	return -1;
}

int timer_destory()
{
	if (mh != NULL) {
		min_heap_dtor(mh);
	}

	if (tl != NULL) {
		list_free(tl);
	}

	if (ml != NULL) {
		m_list_free(ml);
	}

	return 0;
}

//-----------------------------------------------------
struct timer* timer_new() {
	struct timer* t = (struct timer*)malloc(sizeof(struct timer));
	return t;
}

int timer_free(struct timer* t)
{
	if (t == NULL) {
		return 0;
	}

	free(t);
	return 0;
}

//-------------------------------
int timer_set(struct timer* t, void (*cb)(void*), void* arg)
{
	if (t == NULL) {
		return -1;
	}

	t->cb = cb;
	t->arg = arg;
	return 0;
}

int timer_add(struct timer* t, struct timeval* tv)
{
	if (t == NULL) {
		return -1;
	}

	t->timeout.tv_sec = tv->tv_sec;
	t->timeout.tv_usec = tv->tv_usec;

	return min_heap_push(mh, t);
}

int timer_loop()
{
	struct timeval t1;
	struct timeval t2;
	struct timeval tv_p;
	struct timeval* tv;
	tv = &tv_p;
	int ret;

	while (1) {
		timer_get_next_timeout(mh, &tv);

		if (tv != NULL) {
			printf("min: %d, %d\n", tv->tv_sec, tv->tv_usec);
		}

		timer_get_time(&t1);
		ret = (tm->timer_dispath)(tv);

		if (ret == -1) {
			return -1;
		}

		timer_get_time(&t2);
		timer_update(&t1, &t2);
		timer_process_timeout();
		timer_process_actived();
	}
}

static int timer_get_next_timeout(struct min_heap* h, struct timeval** tv)
{
	struct timer* t;
	struct timeval* tv_p = *tv;
	t = min_heap_top(mh);

	if (t == NULL) {
		*tv = NULL;
		return 0;
	}

	(tv_p)->tv_sec = t->timeout.tv_sec;
	(tv_p)->tv_usec = t->timeout.tv_usec;
	return 0;
}

static int timer_get_time(struct timeval* tv)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
		return (-1);
	}

	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / 1000;

	return 0;
}

static int timer_update(struct timeval* s, struct timeval* e)
{
	struct timer** pev;
	unsigned int size;
	struct timeval delta_t;
	struct timeval res;
	struct timeval* ev_tv;
	int tmp = 0;

	pev = mh->p;
	size = mh->n;
	printf("size = %d\n", size);
	printf("start: %d, %d	end: %d, %d\n", s->tv_sec, s->tv_usec, e->tv_sec, e->tv_usec);
	util_timersub(e, s, &delta_t);

	for (; size-- > 0; ++pev) {
		ev_tv = &(**pev).timeout;
		//printf("----delta----%d, %d\n", delta_t.tv_sec, delta_t.tv_usec);
		//printf("----life----%d, %d\n", ev_tv->tv_sec, ev_tv->tv_usec);
		util_timersub(ev_tv, &delta_t, &res);
		ev_tv->tv_sec = res.tv_sec;
		ev_tv->tv_usec = res.tv_usec;
		//printf("----res: %d, %d, %d\n", res.tv_sec, res.tv_usec, tmp++);
	}

	return 0;
}

static int timer_process_timeout()
{
	struct timer* ev;
	struct timeval tv = {0, 0};//0

	while ((ev = min_heap_top(mh))) {
		if (util_timersmaller(&tv, &ev->timeout)) {
			break;
		}

		min_heap_erase(mh, ev);

		list_insert(tl, ev);
	}

	return 0;
}

static int timer_process_actived()
{
	//printf("---timer_process_actived---\n");
	struct timer* p;
	struct timer* n;

	//printf("----len= %d\n", tl->len);
	if (tl->len == 0) {
		return 0;
	}

	p = tl->head;

	while (p != NULL) {
		n = p->next;
		(p->cb)(p->arg);
		list_remove(tl, p);
		p = n;
	}

	return 0;
}

//------------------------------------------
// multi-call
//------------------------------------------
struct timer_wrapper* timer_wrapper_new() {
	struct timer_wrapper* mt = (struct timer_wrapper*)malloc(sizeof(struct timer_wrapper));
	return mt;
}

int timer_wrapper_free(struct timer_wrapper* mt)
{
	if (mt == NULL) {
		return 0;
	}

	free(mt);
	return 0;
}

static void multi_cb(void* arg)
{
	struct timer_wrapper* mt = (struct timer_wrapper*)arg;
	void (*cb)(void*);
	cb = mt->cb;
	void* cb_arg = mt->arg;
	double timeout = mt->timeout;
	cb(cb_arg);

	struct timeval tv;
	tv.tv_sec = (int)timeout;
	tv.tv_usec = (timeout - (int)timeout) * 1000000;
	timer_set(&(mt->st), multi_cb, &(mt->st));//multi_cb自身作为回调
	timer_add(&(mt->st), &tv);
}

static void once_cb(void* arg)
{
	struct timer_wrapper* mt = (struct timer_wrapper*)arg;
	void (*cb)(void*);
	cb = mt->cb;
	void* cb_arg = mt->arg;
	double timeout = mt->timeout;
	cb(cb_arg);

	m_list_remove(ml, mt);
	timer_wrapper_free(mt);
}

//---------------
//interface
//---------------
int multi_call(double timeout, void (*func)(void*), void* arg)
{
	struct timer_wrapper* mt = timer_wrapper_new();

	if (mt == NULL) {
		return -1;
	}

	mt->cb = func;
	mt->arg = arg;
	mt->timeout = timeout;
	struct timeval tv;
	tv.tv_sec = (int)timeout;
	tv.tv_usec = (timeout - (int)timeout) * 1000000;
	timer_set(&(mt->st), multi_cb, &(mt->st));
	timer_add(&(mt->st), &tv);

	//save to the global container
	m_list_insert(ml, mt);

	return 0;
}

int once_call(double timeout, void (*func)(void*), void* arg)
{
	struct timer_wrapper* mt = timer_wrapper_new();

	if (mt == NULL) {
		return -1;
	}

	mt->cb = func;
	mt->arg = arg;
	mt->timeout = timeout;
	struct timeval tv;
	tv.tv_sec = (int)timeout;
	tv.tv_usec = (timeout - (int)timeout) * 1000000;
	timer_set(&(mt->st), once_cb, &(mt->st));
	timer_add(&(mt->st), &tv);

	//save to the global container
	m_list_insert(ml, mt);

	return 0;
}
