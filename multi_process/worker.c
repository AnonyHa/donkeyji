#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int fuck;

int worker_init()
{}

int worker_cycle()
{
	worker_init();

	for (;;) {
		printf("my pid = %d\n", getpid());
		printf("global = %d\n", fuck);
		sleep(1);
	}

	return 0;
}
