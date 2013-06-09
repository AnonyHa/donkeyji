#include <iom.h>

int server_init()
{
	int sock = socket();
	im->add(sock, READ);
}

int server_dispatch()
{
	timer_update();
	im->dispatch(timeout);
	timer_proc();
}
