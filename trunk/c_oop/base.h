#ifndef _BASE_HEAD_H
#define _BASE_HEAD_H

#define HEAD	\
	int type;	\
	int (*f)(struct base_head* p)	\

struct base_head
{
	HEAD;
};

#endif
