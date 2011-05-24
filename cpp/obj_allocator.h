#ifndef OBJ_ALLOCATOR_H_
#define OBJ_ALLOCATOR_H_

#include <vector>

template<typename T>
class ObjAllocator
{
public:
    ObjAllocator();
    ~ObjAllocator();

    bool Reserve(int size);

    int GetNewObj();

    T* GetObj(int id);

private:
    void Release();

private:
    std::vector<T*> _pool;
    int _num_used_obj;
};


template<typename T>
ObjAllocator<T>::ObjAllocator()
{
    _pool.clear();
    _num_used_obj = 0;
}

template<typename T>
ObjAllocator<T>::~ObjAllocator()
{
    Release();
}

template<typename T>
void ObjAllocator<T>::Release()
{
    int num_obj = _pool.size();
    for (int i=0; i<num_obj; ++i) {
        if (_pool[i] != NULL) {
            delete _pool[i];
        }
    }
}

template<typename T>
int ObjAllocator<T>::GetNewObj()
{
    int num_obj = _pool.size();
    if (_num_used_obj >= num_obj) {
        T*  t = new T;
        if (t == NULL) {
            return (_num_used_obj + 1);
        }

        _pool.push_back(t); 
    }


    int id = _num_used_obj;
    ++_num_used_obj;

    return id;
}

template<typename T>
T* ObjAllocator<T>::GetObj(int id)
{
    if (id < _num_used_obj) {
        return _pool[id];
    } else {
        return NULL;
    }
}

#endif
