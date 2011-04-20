#include <gtest/gtest.h>
#include "obj_a.h"
#include "obj_b.h"
#include <iostream>
using namespace std;

TEST(ObjMgrTest, Basic)
{
	obj_a* oa = obj_a::instance();
	ASSERT_TRUE(oa != NULL);

	// generate new obj_b
	int ob0_id = oa->get_new_obj_b();
	int ob1_id = oa->get_new_obj_b();

	cout<<"ob0_id = "<<ob0_id<<endl;
	cout<<"ob1_id = "<<ob1_id<<endl;

	// get specified obj_b
	obj_b* ob0 = obj_b::get_obj_b_by_id(ob0_id);
	obj_b* ob1 = obj_b::get_obj_b_by_id(ob1_id);

	ASSERT_TRUE(ob0 != NULL);
	ASSERT_TRUE(ob1 != NULL);

	// print id
	cout<<ob0->get_id()<<endl;
	cout<<ob1->get_id()<<endl;

	obj_b* ob99 = obj_b::get_obj_b_by_id(99);
	ASSERT_TRUE(ob99 == NULL);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
