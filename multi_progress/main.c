#include "conf.h"
#include "log.h"
#include "server.h"
#include "process.h"
//---------------------------------------------

int main()
{
	conf_init();

	log_init();

	event_init();//必须先初始化，否则后面有依赖

	signal_init();//依赖libevent

	server_init();//完成server对象的创建，listen socket创建

	daemon_run();//是否daemon

	create_pidfile();//创建pid文件

	master_cycle();//master的逻辑

	return 0;
}
