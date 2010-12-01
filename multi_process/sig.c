#include "sig.h"

static volatile sig_atomic_t sig_child = 0;

static void _sig_handler(int sig)
{
	sig_child = 1;
}

int sig_register()
{
	signal(SIGCHLD, _sig_handler);
}

int sig_process()
{
	if (sig_child == 1) {
		sig_child = 0;
		master_spawn_worker();
	}
	return 0;
}
