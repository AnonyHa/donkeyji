#ifndef _BASE_HEAD_H
#define _BASE_HEAD_H

#define HEAD	\
	int type;	\
	int (*f)(struct base_head* p)	\

struct base_head
{
	HEAD;
};//相当于基类，f相当于基类的纯虚函数

#endif
