#include <stdio.h>
#include "mod.h"

extern struct mod_t* all_mods[];

int load_all_mods()
{
	int i;
	int ret;

	for (i=0; all_mods[i]!=NULL; i++) {
		struct mod_t* mod = (struct mod_t*)all_mods[i];

		if (mod->mod_init != NULL) {
			ret = mod->mod_init();

			if (ret < 0) {
				return -1;
			}
		}
	}

	return 0;
}

int run_all_mods()
{
	int i;
	int ret;

	for (i=0; all_mods[i]!=NULL; i++) {
		struct mod_t* mod = (struct mod_t*)all_mods[i];

		if (mod->mod_run != NULL) {
			ret = mod->mod_run();

			if (ret < 0) {
				return -1;
			}
		}
	}

	return 0;
}


int exit_all_mods()
{
	int i;
	int ret;

	for (i=0; all_mods[i]!=NULL; i++) {
		struct mod_t* mod = (struct mod_t*)all_mods[i];

		if (mod->mod_exit != NULL) {
			ret = mod->mod_exit();

			if (ret < 0) {
				return -1;
			}
		}
	}

	return 0;
}
