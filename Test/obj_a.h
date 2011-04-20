#ifndef _OBJ_A_H
#define _OBJ_A_H

#include <vector>


class obj_a
{
private:
	obj_b();
	~obj_b();
public:
	static obj_a* instance():
public:
	int get_new_obj_b();
	obj_a* get_obj_b_by_id(int id);
private:
	void release();
private:
	std::vector<obj_a*> _pool;
	int _used;
private:
	static obj_a* _inst;
};

#endif
