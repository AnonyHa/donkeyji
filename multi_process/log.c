#include "conf.h"
#include "log.h"

LOG_LEVEL log_level = LOG_MSG;
char* log_file = NULL; 
char* log_dir = NULL;
int log_fd = -1

int log_init()
{
	log_level = cfg->log_level;
	strcpy(log_file, cfg->log_file);
	strcpy(log_dir, cfg->log_dir);
	log_fd = open("./log.txt", O_APPEND|O_WRONLY|O_CREAT, 0644);
	if (log_fd == -1)
		return -1;
	return 0;
}

int log_msg()
{}

int log_warn()
{}

int log_debug()
{}

int log_err()
{}
