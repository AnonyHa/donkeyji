#ifndef _LOG_H
#define _LOG_H

typedef enum {
	LOG_MSG,
	LOG_WARN,
	LOG_DEBUG,
	LOG_ERROR,
} LOG_LEVEL;
;

void log_init();
void log_destroy();

void log_warn();
void log_msg(const char* filename, unsigned int line, const char* fmt, ...);
void log_debug();
void log_err();

#endif
