#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#include "conf.h"
#include "log.h"


static void _log_output(int log_level, const char* msg);
static void _log_warn_helper(int log_level, int log_errno, const char* filename, unsigned int line, const char* fmt, va_list ap);

LOG_LEVEL log_level = LOG_MSG;
char* log_file = NULL; 
char* log_dir = NULL;
int log_fd = -1;

int log_init()
{
	log_level = cfg->log_level;
	printf("cfg->log_file = %s\n", cfg->log_file);
	printf("cfg->log_dir = %s\n", cfg->log_dir);
	log_file = (char*)malloc(20);
	log_dir = (char*)malloc(20);
	strcpy(log_file, cfg->log_file);
	strcpy(log_dir, cfg->log_dir);
	log_fd = open(log_file, O_APPEND|O_WRONLY|O_CREAT, 0644);
	if (log_fd == -1)
		return -1;
	return 0;
}

int log_destroy()
{}

static int evutil_vsnprintf(char* buf, size_t buflen, const char* fmt, va_list ap)
{
	int r = vsnprintf(buf, buflen, fmt, ap);
	buf[buflen-1] = '\0';//字符串结尾
	return r;
}

static int evutil_snprintf(char* buf, size_t buflen, const char* fmt, ...)
{
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = evutil_vsnprintf(buf, buflen, fmt, ap);
	va_end(ap);
	return r;
}


static void _log_warn_helper(int log_level, int log_errno, const char* filename, unsigned int line, const char* fmt, va_list ap)
{
	char buf[1024];
	//memset(buf, 0, 1024);
	size_t len;
	int ret;
	int i;


	if (fmt != NULL) {
		evutil_vsnprintf(buf, sizeof(buf), fmt, ap);
	} else {
		buf[0] = '\0';
	}

	/*
	if (log_errno > 0) {
	}
	*/

	/*
	len = strlen(buf);
	evutil_snprintf(buf+len, sizeof(buf)-len, ": %s, %s", filename, line);
	*/

	_log_output(log_level, buf);
}
//打印出log信息
static void _log_output(int log_level, const char* msg)
{
	char time_buf[21];
	char buf[1024];
	time_t timep;
	struct tm* p;
	const char* level_str;
	switch (log_level) {
	case LOG_DEBUG:
		level_str = "DEBUG";
		break;
	case LOG_MSG:
		level_str = "MSG";
		break;
	case LOG_WARN:
		level_str = "WARN";
		break;
	case LOG_ERROR:
		level_str = "ERR";
		break;
	default:
		level_str = "???";
		break;
	}
	//get current time
	time(&timep);
	p = localtime(&timep); 
	if (p == NULL) time_buf[0] = '\0';
	sprintf(
		(char*)time_buf, "%d-%d-%d %d:%d:%d",
		p->tm_year + 1900, p->tm_mon + 1,
		p->tm_mday, p->tm_hour,
		p->tm_min, p->tm_sec
	);
	//time_buf[20] = '\0';//sprintf会自动加上'\0'
	//fprintf(stderr, "[%s]  [%s]  %s\n", time_buf, level_str, msg);//实际的打印输出语句
	evutil_snprintf(buf, 1024, "[%s]  [%s]  %s\n", time_buf, level_str, msg);
	int len = strlen(buf);
	write(log_fd, buf, len);
	printf("%s", buf);
}



int log_msg(const char* filename, unsigned int line, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_log_warn_helper(LOG_MSG, errno, filename, line, fmt, ap);
	va_end(ap);
	return 0;
}

int log_warn()
{}

int log_debug()
{}

int log_err()
{}

