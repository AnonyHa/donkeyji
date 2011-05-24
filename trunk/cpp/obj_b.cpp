#include "obj_b.h"
#include "obj_a.h"

obj_b::obj_b()
{
	_id = -1;//invalid ID
}

obj_b::~obj_b()
{}

obj_b* obj_b::get_obj_b_by_id(int id)
{
	obj_a* inst = obj_a::instance();
	if (inst == NULL)
		return NULL;
	obj_b* oo = inst->get_obj_b_by_id(id);
	return oo;
}

void obj_b::set_id(int id)
{
	_id = id;
}

int obj_b::get_id()
{
	return _id;
}
