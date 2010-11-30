#ifndef _EXTEND_H
#define _EXTEND_H

#include "base.h"

typedef struct data_integer {
	HEAD;
	int value;
}data_integer;//相当于struct data_base的子类


//只向外暴露这个构造函数，而纯虚函数的实现并不暴露
struct data_integer* data_integer_new();
#endif
