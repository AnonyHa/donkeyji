#include <stdio.h>
#include "mod.h"

extern struct mod_t mod_a;
extern struct mod_t mod_b;

struct mod_t* all_mods[] = {
	&mod_a,
	&mod_b,
	NULL
};
