#include <mp.h>

#include "server.h"
#include "process.h"
#include "conf.h"

#include "log.h"

//静态函数declaration
static void sig_register();
static void sig_callback(int sig, short event, void* arg);
static void _sig_callback_master(int sig, short event, void* arg);
static void _sig_callback_worker(int sig, short event, void* arg);

static void master_init();
static void master_start_worker();
static void master_exit();
static pid_t master_spawn_worker();

static void worker_init();
static void worker_cycle();
static void worker_exit();

//to identify child / parent
int is_child = 0;

//-------------------------------------------------------
//fork出子进程之前注册信号
static void 
sig_register()
{
	/*
	int i;
	struct event* se;
	int SIG[] = {SIGINT, SIGTERM, SIGCHLD, SIGHUP};
	for (i=0; i<sizeof(SIG)/sizeof(int); i++) {
		log_msg(__FILE__, __LINE__, "***************** i=%d", i);
		se = (struct event*)calloc(1, sizeof(struct event));
		assert(se);
		log_msg(__FILE__, __LINE__, "se = %x, size = %d, i=%d", se, sizeof(struct event), i);
		signal_set(se, SIG[i], sig_callback, se);
		log_msg(__FILE__, __LINE__, "----------------");
		signal_add(se, NULL);
	}
	*/
}

//父子进程处理信号逻辑不一样
static void 
sig_callback(int sig, short event, void* arg)
{
	if (is_child == 1) {
		_sig_callback_worker(sig, event, arg);
	} else {
		_sig_callback_master(sig, event, arg);
	}
}

static void 
_sig_callback_master(int sig, short event, void* arg)
{
	int stat;
	int ret;
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		ret = wait(&stat);
		if (ret == 0) {
			master_spawn_worker();
		} else if (ret == -1) {}
		break;
	case SIGINT:
	case SIGTERM:
		log_msg(__FILE__, __LINE__, "SIGINT/SIGTERM caught");
		master_exit();
		break;
	case SIGHUP:
		//conf_init();
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	}
}

static void 
_sig_callback_worker(int sig, short event, void* arg)
{
	log_msg(__FILE__, __LINE__, "worker %d callback", getpid());
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		break;
	case SIGINT:
	case SIGTERM:
		log_msg(__FILE__, __LINE__, "SIGINT/SIGTERM caught");
		worker_exit();
		break;
	case SIGHUP:
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	}
}

//-----------------------------------------------------------

static void 
master_init()
{
	log_msg(__FILE__, __LINE__, "master init succeed");
	sig_register();
}

void 
master_cycle()
{
	//master init
	master_init();

	//start worker
	master_start_worker();

	//libevent
	event_dispatch();
}

static void 
master_exit()
{
	//清理资源的代码需要修改
	log_msg(__FILE__, __LINE__, "master kill worker");
	server_destroy();
	log_destroy();
	conf_destroy();

	//kill all process
	kill(0, SIGINT);//self and childs
	exit(0);
}

static void 
master_start_worker()
{
	int i = 0;
	for (i=0; i<3; i++) {
		master_spawn_worker();
		log_msg(__FILE__, __LINE__, "master start the %d worker", i);
	}
	log_msg(__FILE__, __LINE__, "master start worker succeed");
}

static pid_t 
master_spawn_worker()
{
	pid_t ret = fork();
	switch (ret) {
	case -1:
		return -1;
	case 0:
		is_child = 1;//标示为child进程
		log_msg(__FILE__, __LINE__, "come into the worker process pid = %d", getpid());
		worker_cycle();// a loop never break
		break;//不会执行到这里
	default:
		break;
	}	
}


//----------------------------------------------------------

static void 
worker_init()
{
	//init libevent, and add listen socket event
	server_network_register();
	log_msg(__FILE__, __LINE__, "worker init succeed");
}

static void 
worker_cycle()
{
	worker_init();

	log_msg(__FILE__, __LINE__, "begin to event_dispath");
	event_dispatch();
}

static void 
worker_exit()
{
	//这里要修改
	server_destroy();
	log_destroy();
	conf_destroy();

	exit(0);
}
