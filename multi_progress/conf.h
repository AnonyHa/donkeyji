#ifndef _CONF_H
#define _CONF_H

typedef struct _config
{
	int port;
	int max_fds;

	int log_level;
	char* log_file;
	char* log_dir;

	char* doc_root;
}config;

extern config* cfg; 

void conf_init();
void conf_print();
void conf_free();

#endif
