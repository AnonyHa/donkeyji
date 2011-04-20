#ifndef _OBJ_B_H
#define _OBJ_B_H

class obj_b
{
public:
	obj_b();
	~obj_b();
public:
	static obj_b* get_obj_b_by_id(int id);
public:
	void set_id(int id);
	int get_id();
private:
	int _id;
};
#endif
