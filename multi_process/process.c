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
#include "conf.h"

#include "log.h"

//��̬����declaration
static int sig_register();
static void sig_callback(int sig, short event, void* arg);
static void _sig_callback_master(int sig, short event, void* arg);
static void _sig_callback_worker(int sig, short event, void* arg);

static int master_init();
static int master_start_worker();
static int master_exit();
static int master_spawn_worker();

static int worker_init();
static int worker_cycle();
static int worker_exit();

//to identify child / parent
int is_child = 0;

//-------------------------------------------------------
//fork���ӽ���֮ǰע���ź�
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

//���ӽ��̴����ź��߼���һ��
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
		master_exit();
		break;
	case SIGHUP:
		//conf_init();
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
		worker_exit();
		break;
	case SIGHUP:
		log_msg(__FILE__, __LINE__, "SIGHUP caught");
		break;
	}
}

//-----------------------------------------------------------

static int master_init()
{
	log_msg(__FILE__, __LINE__, "master init succeed");
	sig_register();
}

int master_cycle()
{
	//master init
	master_init();

	//start worker
	master_start_worker();

	//libevent
	event_dispatch();

	return 0;
}

static int master_exit()
{
	//������Դ�Ĵ�����Ҫ�޸�
	log_msg(__FILE__, __LINE__, "master kill worker");
	server_destroy();
	log_destroy();
	conf_destroy();

	//kill all process
	kill(0, SIGINT);//self and childs
	exit(0);
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
		is_child = 1;//��ʾΪchild����
		log_msg(__FILE__, __LINE__, "come into the worker process pid = %d", getpid());
		worker_cycle();// a loop never break
		break;//����ִ�е�����
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

static int worker_exit()
{
	//����Ҫ�޸�
	server_destroy();
	log_destroy();
	conf_destroy();

	exit(0);
}
