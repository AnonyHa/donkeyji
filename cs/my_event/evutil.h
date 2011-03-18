#ifndef _EVUTIL_H
#define _EVUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdarg.h>

#include "event.h"

#define EVUTIL_SET_SOCKET_ERROR(errcode)	\
	do {errno = (errcode);} while (0)

	int evutil_socketpair(int family, int type, int protocol, int fd[2]);

	int evutil_vsnprintf(char* buf, size_t buflen, const char* fmt, va_list ap);


	int evutil_snprintf(char* buf, size_t buflen, const char* fmt, ...);

//void evutil_timeradd(&now, tv, &ev->ev_timeout);

#define evutil_timerclear(tvp) timerclear(tvp)

#ifdef __cplusplus
}
#endif

#endif
