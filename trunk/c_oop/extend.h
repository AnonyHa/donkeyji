#ifndef _EXTEND_H
#define _EXTEND_H

#include "base.h"

struct extend
{
	HEAD;
	int ext;
};

//interface
struct extend* extend_new();
#endif
