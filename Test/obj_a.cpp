#include "obj_a.h"


static obj_a::instance()
{
}

obj_b::obj_b()
{
	_pool.clear();
	_used = 0;
}

obj_b::~obj_b()
{
	release();
}

int obj_b::get_new_obj_b()
{
	int cnt = _pool.size();
	if (_used >= cnt) {
		obj_a* t = new obj_a;
		if (t == NULL) {
			return _used + 1;
		}
		_pool.push_back(t);
	}
}

void obj_b::release()
{
	int cnt = _pool.size();
	for (int i=0; i<cnt; i++) {
		if (_pool[i] != NULL) {
			delete _pool[i];
		}
	}
}

obj_b* obj_a::get_obj_b_by_id(int id)
{

}
