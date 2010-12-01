#ifndef _LOG_H
#define _LOG_H

typedef enum {
	LOG_MSG,
	LOG_WARN,
	LOG_DEBUG,
	LOG_ERROR,
}LOG_LEVEL;
;

int log_init();

int log_warn();
int log_msg();
int log_debug();
int log_err();

#endif
