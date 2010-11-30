#ifndef _EXTEND_H
#define _EXTEND_H

#include "base.h"

struct extend
{
	HEAD;
	int ext;
};//相当于struct base_head的子类


//interface
struct extend* extend_new();
#endif
