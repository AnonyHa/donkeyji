#include "obj_allocator.h"

#include <string>
#include <map>
#include <iostream>
using namespace std;

int main()
{
    ObjAllocator<int> obj_allocator;

    int idx0 = obj_allocator.GetNewObj();
    int idx1 = obj_allocator.GetNewObj();
	cout<<"idx0="<<idx0<<endl;
	cout<<"idx1="<<idx1<<endl;

    int* obj0 = obj_allocator.GetObj(idx0);
    int* obj1 = obj_allocator.GetObj(idx1);

    *obj0 = 0;
    *obj1 = 1;

    obj0 = obj_allocator.GetObj(idx0);
    obj1 = obj_allocator.GetObj(idx1);

	cout<<"obj0="<<*obj0<<endl;
	cout<<"obj1="<<*obj1<<endl;

   int* obj100 = obj_allocator.GetObj(100);

   return 0;
}
