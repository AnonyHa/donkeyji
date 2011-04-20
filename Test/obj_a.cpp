#include "obj_a.h"
#include "obj_b.h"

obj_a* obj_a::_inst = NULL;

obj_a* obj_a::instance()
{
	// not thread safe
	if (_inst == NULL) {
		_inst = new obj_a;
	}
	return _inst;
}

obj_a::obj_a()
{
	_pool.clear();
	_used = 0;
}

obj_a::~obj_a()
{
	release();
}

int obj_a::get_new_obj_b()
{
	int cnt = _pool.size();
	if (_used >= cnt) {
		obj_b* t = new obj_b;
		if (t == NULL) {
			return _used + 1;
		}
		t->set_id(_used);
		_pool.push_back(t);
	}
	int id = _used;
	_used++;
	return id;
}

void obj_a::release()
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
	if (id < _used) {
		return _pool[id];
	} else {
		return NULL;
	}
}
