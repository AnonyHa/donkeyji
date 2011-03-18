#include <stdio.h>
#include <stdlib.h>
#include "fr.h"
#include "dep.h"

int main()
{
	int ret;
	//---------------------
	//init
	//---------------------
	ret = dep_init();

	if (ret < 0) {
		goto deal_error;
	}

	ret = load_all_mods();

	if (ret < 0) {
		goto deal_error;
	}

	//---------------------
	//run
	//---------------------
	ret = dep_run();

	if (ret < 0) {
		goto deal_error;
	}

	ret = run_all_mods();

	if (ret < 0) {
		goto deal_error;
	}

	//---------------------
	//exit
	//---------------------
	ret = dep_exit();

	if (ret < 0) {
		goto deal_error;
	}

	ret = exit_all_mods();

	if (ret < 0) {
		goto deal_error;
	}

	return 0;
	//---------------------
	//handle error
	//---------------------
deal_error:
	printf("deal error\n");
	exit(1);
}
