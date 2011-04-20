#include "obj_a.h"
#include "obj_b.h"
using namespace std;

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
}

obj_a::~obj_a()
{
	release();
}

int obj_a::get_new_obj_b()
{
	int cnt = _pool.size();

	obj_b* t = new obj_b;
	if (t == NULL)
		return cnt+1;//beyond [0, cnt-1]
	t->set_id(cnt);
	_pool.push_back(t);
	return cnt;//id
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
	int cnt = _pool.size();
	if (id < cnt && id >= 0) {
		return _pool[id];
	} else {
		return NULL;
	}
}
