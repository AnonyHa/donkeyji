#include "conf.h"
#include "log.h"
#include "server.h"
#include "process.h"
//---------------------------------------------

int main()
{
	conf_init();

	log_init();

	//���server����Ĵ�����listen socket�Ĵ�����libevent�ĳ�ʼ��
	server_init();

	master_cycle();

	return 0;
}
