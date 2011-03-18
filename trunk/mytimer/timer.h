#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

struct min_heap;
struct timer {
	unsigned int min_heap_idx;
	struct timeval timeout;
	void (*cb) (void* arg);
	void* arg;

	// for list
	struct timer* next;
	struct timer* prev;
};

struct timer_model {
	int (*timer_dispath)(struct timeval*);
};

struct timer_wrapper {
	struct timer st;
	void (*cb)(void*);
	void* arg;
	double timeout;

	//for list
	struct timer_wrapper* next;
	struct timer_wrapper* prev;
};

//--------------------------------------------

struct timer* timer_new();
int timer_free(struct timer* t);
int timer_set(struct timer* t, void (*cb)(void*), void* arg);
int timer_add(struct timer* t, struct timeval* tv);
int timer_del();
int timer_loop();

//--------------------------------------------

static int timer_dispath(struct timeval* tv);
static int timer_update(struct timeval* s, struct timeval* e);
static int timer_process_timeout();
static int timer_process_actived();
static int timer_get_time(struct timeval* tv);
static int timer_get_next_timeout(struct min_heap* h, struct timeval** tv);

//--------------------------------------------
static void multi_cb(void* arg);
static void once_cb(void* arg);

//--------------------------------------------
struct timer_wrapper* timer_wrapper_new();
int timer_wrapper_free(struct timer_wrapper* mt);
int multi_call(double timeout, void (*func)(void*), void* arg);
int once_call(double timeout, void (*func)(void*), void* arg);

//-----------
//module init
//-----------
int timer_init();
int timer_destory();

#endif
