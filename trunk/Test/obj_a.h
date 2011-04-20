#ifndef _OBJ_A_H
#define _OBJ_A_H

#include <vector>
class obj_b;

class obj_a
{
public:
	static obj_a* instance();
	int get_new_obj_b();
	obj_b* get_obj_b_by_id(int id);
	~obj_a();
private:
	void release();
private:
	std::vector<obj_b*> _pool;
	static obj_a* _inst;
private:
	obj_a();
	obj_a(const obj_a& other);
	obj_a& operator=(const obj_a& other);
};

#endif
