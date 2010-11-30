#ifndef _UTIL_H 
#define _UTIL_H

#include <time.h>

void util_timersub(struct timeval* a, struct timeval* b, struct timeval* res)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	if (res->tv_usec < 0) {
		res->tv_sec--;
		res->tv_usec += 1000000;
	}
}


// whether a < b ?
int util_timersmaller(struct timeval* a, struct timeval* b)
{
	if (a->tv_sec == b->tv_sec) {
		if (a->tv_usec < b->tv_usec)
			return 1;
		else
			return 0;
	} else {
		if (a->tv_sec < b->tv_sec)
			return 1;
		else
			return 0;
	}
}

#endif
