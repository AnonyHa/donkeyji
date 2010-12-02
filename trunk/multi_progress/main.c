#include "conf.h"
#include "log.h"
#include "server.h"
#include "process.h"
//---------------------------------------------

int main()
{
	conf_init();

	log_init();

	//完成server对象的创建，listen socket的创建，libevent的初始化
	server_init();

	master_cycle();

	return 0;
}
