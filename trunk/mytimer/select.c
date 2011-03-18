#include <stdio.h>
#include "timer.h"

static int select_dispatch(struct timeval* tv);

struct timer_model select_model = {
	select_dispatch
};

static int select_dispatch(struct timeval* tv)
{
	//the first argument should be 0
	int ret = select(0, NULL, NULL, NULL, tv);

	if (ret < 0) {
		return -1;
	}

	return 0;
}
