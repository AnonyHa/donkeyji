#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <event.h>

#include "server.h"
#include "process.h"

#include "sig.h"
#include "log.h"

//静态函数declaration
static int sig_register();
static void sig_callback(int sig, short event, void* arg);
static void _sig_callback_master(int sig, short event, void* arg);
static void _sig_callback_worker(int sig, short event, void* arg);

static int master_init();
static int master_start_worker();
static int master_kill_worker();
static int master_spawn_worker();
static int master_destroy();

static int worker_init();
static int worker_cycle();

//to identify child / parent
int is_child = 0;

//-------------------------------------------------------
//fork出子进程之前注册信号
static int sig_register()
{
	int i;
	struct event* se;
	int SIG[] = {SIGINT, SIGTERM, SIGCHLD, SIGHUP};
	for (i=0; i<sizeof(SIG)/sizeof(int); i++) {
		se = (struct event*)calloc(1, sizeof(struct event));
		log_msg(__FILE__, __LINE__, "se = %x, size = %d, i=%d", se, sizeof(struct event), i);
		assert(se);
		signal_set(se, SIGINT, sig_callback, se);
		signal_add(se, NULL);
	}
}

//父子进程处理信号逻辑不一样
static void sig_callback(int sig, short event, void* arg)
{
	if (is_child == 1) {
		_sig_callback_worker(sig, event, arg);
	} else {
		_sig_callback_master(sig, event, arg);
	}
}

static void _sig_callback_master(int sig, short event, void* arg)
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
		master_kill_worker();//kill child
		exit(1);//quit it self
		break;
	case SIGHUP:
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	}
}

static void _sig_callback_worker(int sig, short event, void* arg)
{
	log_msg(__FILE__, __LINE__, "worker %d callback", getpid());
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		break;
	case SIGINT:
	case SIGTERM:
		log_msg(__FILE__, __LINE__, "SIGINT/SIGTERM caught");
		exit(1);
		break;
	case SIGHUP:
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	}
}

//-----------------------------------------------------------

static int master_init()
{
	sig_register();
	log_msg(__FILE__, __LINE__, "master init succeed");
}

int master_cycle()
{
	int stat;
	//master init
	master_init();

	//start worker
	master_start_worker();

	//libevent
	event_dispatch();

	return 0;
}

static int master_kill_worker()
{
	log_msg(__FILE__, __LINE__, "master kill worker");
	//kill all process
	kill(0, SIGINT);//self and childs
}

static int master_destroy()
{
	server_destroy();
	log_destroy();
	conf_destroy();
	return 0;
}

static int master_start_worker()
{
	int i = 0;
	for (i=0; i<3; i++) {
		master_spawn_worker();
		log_msg(__FILE__, __LINE__, "master start the %d worker", i);
	}
	log_msg(__FILE__, __LINE__, "master start worker succeed");
}

static int master_spawn_worker()
{
	int ret = fork();
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

static int worker_init()
{
	//init libevent, and add listen socket event
	server_network_register();
	log_msg(__FILE__, __LINE__, "worker init succeed");
}

static int worker_cycle()
{
	worker_init();

	log_msg(__FILE__, __LINE__, "begin to event_dispath");
	event_dispatch();

	return 0;
}
