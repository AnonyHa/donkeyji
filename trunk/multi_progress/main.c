#include "conf.h"
#include "log.h"
#include "server.h"
#include "process.h"
//---------------------------------------------

int main()
{
	conf_init();

	log_init();

	event_init();//�����ȳ�ʼ�����������������

	signal_init();//����libevent

	server_init();//���server����Ĵ�����listen socket����

	daemon_run();//�Ƿ�daemon

	create_pidfile();//����pid�ļ�

	master_cycle();//master���߼�

	return 0;
}
