#include <time.h>
#include <iostream>
#include <fstream>
#include "pto_macro.h"
#include "s_login.pb.h"
using namespace std;

extern int reg_msg();
extern int check_reg();

int main()
{
	fstream output("./log", ios::out|ios::trunc|ios::binary);
	reg_msg();
	check_reg();
	S_LOGIN_T st;
	st.set_id(1111);
	st.set_name("huji");
	st.SerializeToOstream(&output);

	S_LOGIN_T st2;
	fstream input("./log", ios::in|ios::binary);
	st2.ParseFromIstream(&input);
	cout<<st2.id()<<endl;
	cout<<st2.name()<<endl;

	/*
	while (1) {
		sleep(1);
	}
	*/
	return 0;
}
