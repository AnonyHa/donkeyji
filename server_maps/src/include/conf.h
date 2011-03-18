#ifndef _CONF_H
#define _CONF_H

typedef struct _config {
	int server_mode;

	int login_client_port;
	int login_realm_port;
	int login_max_conns;

	int log_level;
	char* log_file;

	int is_daemon;
} config;

extern config* cfg;

void conf_init();
void conf_print();
void conf_free();

#endif
