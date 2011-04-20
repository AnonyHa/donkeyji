#include "obj_b.h"
#include "obj_a.h"

obj_b::obj_b()
{
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
