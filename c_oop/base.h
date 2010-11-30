#ifndef _BASE_HEAD_H
#define _BASE_HEAD_H

#define HEAD	\
	int type;	\
	char* buffer;	\
	void (*free)(struct data_base* p)
//注意最后一行没有:

typedef struct data_base{
	HEAD;
}data_base;//相当于基类，f相当于基类的纯虚函数

#endif
