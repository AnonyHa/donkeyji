#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "event.h"
#include "log.h"
#include "evutil.h"
#include "queue.h"

extern struct event_base* current_base;
struct event_base* evsignal_base = NULL;

static void evsignal_cb(int fd, short what, void* arg)
{
	event_debugx("evsignal_cb");
	static char signals[100];
	ssize_t n;
	n = recv(fd, signals, sizeof(signals), 0);

	if (n == -1) {
		event_err(1, "%s: read", __func__);
	}
}

#define FD_CLOSEONEXEC(x) do {\
	if (fcntl(x, F_SETFD, 1) == -1) \
		event_warn("fcntl(%d, F_SETFD)", x);	\
} while (0)


void evsignal_init(struct event_base* base)
{
	int i;

	if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, base->sig.ev_signal_pair) == -1) {
		event_err(1, "%s: socketpair", __func__);
	}

	//event_debugx("2 fds: %d, %d", base->sig.ev_signal_pair[0], base->sig.ev_signal_pair[1]);

	FD_CLOSEONEXEC(base->sig.ev_signal_pair[0]);
	FD_CLOSEONEXEC(base->sig.ev_signal_pair[1]);

	base->sig.sh_old = NULL;
	base->sig.sh_old_max = 0;
	base->sig.evsignal_caught = 0;
	memset(&base->sig.evsigcaught, 0, sizeof(sig_atomic_t)*NSIG);

	for (i=0; i<NSIG; i++) {
		TAILQ_INIT(&base->sig.evsigevents[i]);
	}

	evutil_make_socket_nonblocking(base->sig.ev_signal_pair[0]);

	//event_debugx("sig.ev_signal->ev_fd = %d", base->sig.ev_signal_pair[1]);
	event_set(&base->sig.ev_signal, base->sig.ev_signal_pair[1],
	          EV_READ|EV_PERSIST, evsignal_cb, &base->sig.ev_signal);
	//????
	base->sig.ev_signal.ev_base = base;
	base->sig.ev_signal.ev_flags |= EVLIST_INTERNAL;
}

//这个调用失败导致evsignal_add失败，由此epoll_add失败，由此event_add失败
int _evsignal_set_handler(struct event_base* base, int evsignal, void(*handler)(int))
{
	struct sigaction sa;
	struct evsignal_info* sig = &base->sig;
	void* p;

	if (evsignal >= sig->sh_old_max) {//有新的信号加入
		int new_max = evsignal + 1;
		event_debugx("new signal add");
		p = realloc(sig->sh_old, new_max * sizeof(*sig->sh_old));

		if (p == NULL) {
			event_warn("realloc");
			return -1;
		}

		//*sig->sh_old: sigaction型的指针
		memset( (char*)p + sig->sh_old_max * sizeof(*sig->sh_old), 0,
		        (new_max - sig->sh_old_max) * sizeof(*sig->sh_old) );
		sig->sh_old_max = new_max;
		sig->sh_old = p;
	}

	sig->sh_old[evsignal] = malloc(sizeof(*sig->sh_old[evsignal]));

	if (sig->sh_old[evsignal] == NULL) {
		event_warn("malloc");
		return -1;
	}

	//利用sigaction设置信号处理函数
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);

	if (sigaction(evsignal, &sa, sig->sh_old[evsignal]) == -1) {
		event_warn("sigaction");
		free(sig->sh_old[evsignal]);
		return -1;
	}

	return 0;
}

static void evsignal_handler(int sig)
{
	event_debugx("----catch signal----");

	int save_errno = errno;//????????

	if (evsignal_base == NULL) {
		return;
	}

	evsignal_base->sig.evsigcaught[sig]++;
	evsignal_base->sig.evsignal_caught = 1;//在epoll_dispatch中会用到这个值
	assert(evsignal_base == current_base);

	int n = send(evsignal_base->sig.ev_signal_pair[0], "a", 1, 0);
	errno = save_errno;
}

int evsignal_add(struct event* ev)
{
	event_debugx("evsignal_add");
	int evsignal;
	struct event_base* base = ev->ev_base;
	struct evsignal_info* sig = &ev->ev_base->sig;

	if (ev->ev_events & (EV_READ|EV_WRITE)) {
		event_errx(1, "%s: EV_SIGNAL incompatible use", __func__);
	}

	evsignal = EVENT_SIGNAL(ev);
	assert(evsignal >= 0 && evsignal < NSIG);

	//第一次加入时，需要设置信号处理函数
	//后续加入时，就无需在设置信号处理函数
	if (TAILQ_EMPTY(&sig->evsigevents[evsignal])) {
		event_debugx("first time add signal type");

		if (_evsignal_set_handler(base, evsignal, evsignal_handler) == -1) {
			return -1;    //设置信号处理函数
		}

		evsignal_base = base;

		//sig->ev_signal仅需event_add一次
		if (!sig->ev_signal_added) {
			if (event_add(&sig->ev_signal, NULL) != 0) {
				return -1;
			}

			sig->ev_signal_added = 1;
		}
	}

	TAILQ_INSERT_TAIL(&sig->evsigevents[evsignal], ev, ev_signal_next);
	return 0;
}

void evsignal_process(struct event_base* base)
{
	struct evsignal_info* sig = &base->sig;
	struct event* ev;
	struct event* next_ev;
	sig_atomic_t ncalls;
	int i;

	base->sig.evsignal_caught = 0;

	for (i=1; i<NSIG; i++) {
		ncalls = sig->evsigcaught[i];

		if (ncalls == 0) {
			continue;
		}

		for (ev=TAILQ_FIRST(&sig->evsigevents[i]); ev!=NULL; ev=next_ev) {
			next_ev = TAILQ_NEXT(ev, ev_signal_next);

			if (!(ev->ev_events & EV_PERSIST)) {
				event_del(ev);
			}

			event_active(ev, EV_SIGNAL, ncalls);
		}

		sig->evsigcaught[i] = 0;
	}
}

int evsignal_del(struct event* ev)
{}
