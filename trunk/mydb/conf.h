#ifndef _CONF_H
#define _CONF_H

struct db_cfg {
	//for game svr
	unsigned int _listen_port;

	//lua script
	const char* _script_file;

	//for mysql
	char* _mysql_addr;
	unsigned int _mysql_port;
	char* _mysql_db;
	char* _mysql_user;
	char* _mysql_pwd;
	char* _mysql_unix_socket;
	unsigned int _mysql_client_flag;
};

struct db_cfg* conf_new();

void conf_free(struct db_cfg* conf);

int conf_init(struct db_cfg* conf);
#endif
