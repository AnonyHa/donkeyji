#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "log.h"

static void _warn_helper(int log_level, int log_errno, const char* fmt, va_list ap);
static void event_log(int log_level, const char* msg);

void event_err(int eval, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_ERR, errno, fmt, ap);
	va_end(ap);

	exit(eval);
}

void event_warn(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_WARN, errno, fmt, ap);
	va_end(ap);
}

void event_errx(int eval, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_ERR, -1, fmt, ap);
	va_end(ap);

	exit(eval);
}

void event_warnx(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_WARN, -1, fmt, ap);
	va_end(ap);
}

void event_msgx(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_MSG, -1, fmt, ap);
	va_end(ap);
}

void event_debugx(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(_EVENT_LOG_DEBUG, -1, fmt, ap);//ap传值，而非指针
	va_end(ap);
}

//打印出log信息, private, not interface
static void _warn_helper(int log_level, int log_errno, const char* fmt, va_list ap)
{
	char buf[1024];//log字符串的容器
	size_t len;

	if (fmt != NULL) {
		evutil_vsnprintf(buf, sizeof(buf), fmt, ap);
	} else {
		buf[0] = '\0';    //字符串结尾
	}

	// 遇到出错，在log信息的尾部加上strerror获取的错误信息
	if (log_errno >= 0) {
		len = strlen(buf);//字符串长度

		if (len < sizeof(buf) - 3) {
			evutil_snprintf(buf + len, sizeof(buf) - len, ": %s", strerror(log_errno));//在buf后部加上错误信息
		}
	}

	//实际的输出，根据log_level加上前缀[DEBUG], [MSG], [WARN], [ERR]
	event_log(log_level, buf);
}

//不用默认的fprintf，调用callback来处理log信息
static event_log_cb log_fn = NULL;

void event_set_log_callback(event_log_cb cb)
{
	log_fn = cb;
}

//打印出log信息
static void event_log(int log_level, const char* msg)
{
	if (log_fn) {
		log_fn(log_level, msg);
	} else {
		const char* level_str;

		switch (log_level) {
		case _EVENT_LOG_DEBUG:
			level_str = "DEBUG";
			break;
		case _EVENT_LOG_MSG:
			level_str = "MSG";
			break;
		case _EVENT_LOG_WARN:
			level_str = "WARN";
			break;
		case _EVENT_LOG_ERR:
			level_str = "ERR";
			break;
		default:
			level_str = "???";
			break;
		}

		//get current time
		char time_buf[21];
		time_t timep;
		struct tm* p;
		time(&timep);
		p = localtime(&timep);

		if (p == NULL) {
			time_buf[0] = '\0';
		}

		sprintf(
		    (char*)time_buf, "%d-%d-%d %d:%d:%d",
		    p->tm_year + 1900, p->tm_mon + 1,
		    p->tm_mday, p->tm_hour,
		    p->tm_min, p->tm_sec
		);
		//time_buf[20] = '\0';//sprintf会自动加上'\0'
		fprintf(stderr, "[%s]  [%s]  %s\n", time_buf, level_str, msg);//实际的打印输出语句
	}
}

