#include "afxdb.h"
//#include <libconfig.h>

//static config_t* g_cfg = NULL;

struct db_cfg* conf_new() {
	struct db_cfg* conf = (struct db_cfg*)malloc(sizeof(struct db_cfg));
	return conf;
}

void conf_free(struct db_cfg* conf)
{
	if (conf == NULL) {
		return;
	}

	free(conf);
}

int conf_init(struct db_cfg* conf)
{
	/*
	struct stat st;
	if (g_cfg == NULL) {
		g_cfg = new config_t;
		config_init(g_cfg);
	}

	if (stat(FileName, &st) == -1) {
		return -1;
	}

	if (!config_load_file (g_cfg, FileName)) {
		return -1;
	}
	*/

	/*
	conf->_listen_port = config_lookup_int(g_cfg, "LISTEN_PORT");
	conf->_mysql_addr = config_lookup_string(g_cfg, "MYSQL_ADDR");

	cfgData.ServerId = config_lookup_int(g_cfg, "server_id");
	cfgData.Region = config_lookup_string(g_cfg, "region");
	cfgData.ServerName = config_lookup_string(g_cfg, "server_name");
	cfgData.WorkDir = config_lookup_string(g_cfg, "work_dir");
	cfgData.Port = config_lookup_int(g_cfg, "port");
	cfgData.MaxEvalCost = config_lookup_int(g_cfg, "max_eval_cost");
	}
	*/

	return 1;
}
