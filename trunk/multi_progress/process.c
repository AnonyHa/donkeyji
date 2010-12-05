#include <mp.h>

#include "server.h"
#include "process.h"
#include "conf.h"

#include "log.h"

//静态函数declaration
static void _sig_register();
static void _sig_callback(int sig, short event, void* arg);
static void _sig_callback_master(int sig, short event, void* arg);
static void _sig_callback_worker(int sig, short event, void* arg);

static void _master_set_proctitle(const char* title);
static void _master_init();
static void _master_start_worker();
static void _master_exit();
static pid_t _master_spawn_worker();

static _worker_set_proctitle(const char* title);
static void _worker_init();
static void _worker_cycle();
static void _worker_exit();

//to identify child / parent
int is_child = 0;

//-------------------------------------------------------
//fork出子进程之前注册信号
static void 
_sig_register()
{
	int i;
	struct event* se;
	int SIG[] = {SIGINT, SIGTERM, SIGCHLD, SIGHUP};
	for (i=0; i<sizeof(SIG)/sizeof(int); i++) {
		se = (struct event*)calloc(1, sizeof(struct event));
		assert(se);
		signal_set(se, SIG[i], _sig_callback, se);
		signal_add(se, NULL);
	}
}

//父子进程处理信号逻辑不一样
static void 
_sig_callback(int sig, short event, void* arg)
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
	pid_t pid;
	log_msg(__FILE__, __LINE__, "master sig %d callback", getpid());
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		ret = wait(&stat);
		if (ret == -1) {
			log_msg(__FILE__, __LINE__, "wait error: %s", strerror(errno));
		} else {
			log_msg(__FILE__, __LINE__, "wait child pid = %d", ret);
			pid = _master_spawn_worker();
			log_msg(__FILE__, __LINE__, "spawn one more worker: pid = %d", pid);
		}
		break;
	case SIGINT:
	case SIGTERM:
		log_msg(__FILE__, __LINE__, "SIGINT/SIGTERM caught");
		_master_exit();
		break;
	case SIGHUP:
		//conf_init();
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	default:
		log_msg(__FILE__, __LINE__, "%d caught", sig);
		break;
	}
}

static void 
_sig_callback_worker(int sig, short event, void* arg)
{
	log_msg(__FILE__, __LINE__, "worker sig %d callback", getpid());
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		wait(NULL);
		break;
	case SIGINT:
	case SIGTERM:
		log_msg(__FILE__, __LINE__, "SIGINT/SIGTERM caught");
		_worker_exit();
		break;
	case SIGHUP:
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	default:
		log_msg(__FILE__, __LINE__, "%d caught", sig);
		break;
	}
}

//-----------------------------------------------------------

static void 
_master_init()
{
	log_msg(__FILE__, __LINE__, "master init succeed");
	_sig_register();
	_master_set_proctitle("masterd");
}

static void 
_master_set_proctitle(const char* title)
{

}

void 
master_cycle()
{
	//master init
	_master_init();

	//start worker
	_master_start_worker();

	//libevent
	event_dispatch();
}

static void 
_master_exit()
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
_master_start_worker()
{
	int i = 0;
	pid_t pid;
	//for (i=0; i<3; i++) {
	for (i=0; i<2; i++) {
		pid = _master_spawn_worker();
		log_msg(__FILE__, __LINE__, "master start the %d worker: pid=%d", i, pid);
	}
	log_msg(__FILE__, __LINE__, "master start worker succeed");
}

static pid_t 
_master_spawn_worker()
{
	pid_t ret = fork();
	switch (ret) {
	case -1:
		return -1;
	case 0:
		is_child = 1;//标示为child进程
		log_msg(__FILE__, __LINE__, "come into the worker process pid = %d", getpid());
		_worker_cycle();// a loop never break
		break;//不会执行到这里
	default:
		return ret;
	}	
}


//----------------------------------------------------------

static void 
_worker_init()
{
	//init libevent, and add listen socket event
	server_network_register();
	_worker_set_proctitle("workerd");
	log_msg(__FILE__, __LINE__, "worker init succeed");
}

static 
_worker_set_proctitle(const char* title)
{}

static void 
_worker_cycle()
{
	_worker_init();

	log_msg(__FILE__, __LINE__, "begin to event_dispath");
	event_dispatch();
}

static void 
_worker_exit()
{
	//这里要修改
	server_destroy();
	log_destroy();
	conf_destroy();

	log_msg(__FILE__, __LINE__, "begin to call exit");
	exit(0);
}
