#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "master.h"
#include "worker.h"
#include "sig.h"

//统一的信号处理函数

int master_init()
{
	sig_register();
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
	}
}

int master_spawn_worker()
{
	int ret = fork();
	switch (ret) {
	case -1:
		return -1;
	case 0:
		worker_cycle();// a loop never break
		break;//不会执行到这里
	default:
		break;
	}	
}
