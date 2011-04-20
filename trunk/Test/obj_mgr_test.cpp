//#include <gtest/gtest.h>
#include "obj_a.h"
#include "obj_b.h"
#include <iostream>
using namespace std;

int main()
{
	obj_a* oa = obj_a::instance();
	if (oa == NULL) {
		cout<<"1111"<<endl;
		return -1;
	}

	// generate new obj_b
	int ob1_id = oa->get_new_obj_b();
	int ob2_id = oa->get_new_obj_b();

	cout<<"ob1_id = "<<ob1_id<<endl;
	cout<<"ob2_id = "<<ob2_id<<endl;

	// get specified obj_b
	obj_b* ob1 = obj_b::get_obj_b_by_id(ob1_id);
	obj_b* ob2 = obj_b::get_obj_b_by_id(ob2_id);

	cout<<ob1->get_id()<<endl;
	cout<<ob2->get_id()<<endl;

	return 0;
}
