#include <signal.h>

#include "sig.h"

static volatile sig_atomic_t sig_child = 0;
static volatile sig_atomic_t sig_int = 0;

static void _sig_handler(int sig)
{
	switch (sig) {
	case SIGCHLD:
		log_msg(__FILE__, __LINE__, "SIGCHLD caught");
		sig_child = 1;
		break;
	case SIGINT:
		log_msg(__FILE__, __LINE__, "SIGINT caught");
		sig_int = 1;
		break;
	}
	sig_child = 1;
}

int sig_register()
{
	signal(SIGCHLD, _sig_handler);
	signal(SIGINT, _sig_handler);
}

int sig_process()
{
	if (sig_child == 1) {
		sig_child = 0;
		log_msg(__FILE__, __LINE__, "a child exit");
		master_spawn_worker();
	}
	if (sig_int == 1) {
		sig_int = 0;
		log_msg(__FILE__, __LINE__, "prepare to kill all progress ");
		kill(0, SIGTERM);
	}
	return 0;
}
