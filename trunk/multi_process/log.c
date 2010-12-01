#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"
#include "log.h"

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
