#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "master.h"
#include "worker.h"
#include "sig.h"
#include "log.h"

//统一的信号处理函数

int master_init()
{
	sig_register();
	log_msg(__FILE__, __LINE__, "master init succeed");
}

int master_cycle()
{
	//master init
	master_init();

	//start worker
	master_start_worker();

	//master loop
	while (1) {//main loop of master
		sig_process();
		sleep(1);
	}
	return 0;
}

int master_start_worker()
{
	int i = 0;
	for (i=0; i<3; i++) {
		master_spawn_worker();
		log_msg(__FILE__, __LINE__, "master start the %d worker", i);
	}
	log_msg(__FILE__, __LINE__, "master start worker succeed");
}

int master_spawn_worker()
{
	int ret = fork();
	switch (ret) {
	case -1:
		return -1;
	case 0:
		log_msg(__FILE__, __LINE__, "come into the worker process pid = %d", getpid());
		worker_cycle();// a loop never break
		break;//不会执行到这里
	default:
		break;
	}	
}
