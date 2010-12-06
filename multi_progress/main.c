#include "conf.h"
#include "log.h"
#include "server.h"
#include "process.h"
//---------------------------------------------

int main()
{
	conf_init();

	log_init();

	signal_init();

	//���server����Ĵ�����listen socket�Ĵ�����libevent�ĳ�ʼ��
	server_init();

	daemon_run();

	create_pidfile();

	master_cycle();

	return 0;
}
