#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "evutil.h"
#include "log.h"

int evutil_socketpair(int family, int type, int protocol, int fd[2])
{
	return socketpair(family, type, protocol, fd);
}

int evutil_vsnprintf(char* buf, size_t buflen, const char* fmt, va_list ap)
{
	int r = vsnprintf(buf, buflen, fmt, ap);
	buf[buflen-1] = '\0';//字符串结尾
	return r;
}

int evutil_snprintf(char* buf, size_t buflen, const char* fmt, ...)
{
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = evutil_vsnprintf(buf, buflen, fmt, ap);
	va_end(ap);
	return r;
}

int evutil_make_socket_nonblocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		event_warn("fcntl(O_NONBLOCK)");
		return -1;
	}

	return 0;
}

//void evutil_timeradd(&now, tv, &ev->ev_timeout)
//{}
