#include "kv_parser.h"

#include <string>
#include <map>
#include <iostream>

using namespace std;

int main()
{
    const char* str = "k1=v1; k2 =  v2  ; k3 =v3; k4=v4; k3=v5;  ";

    KVParser kv_parser(str);

    cout<<kv_parser.get("k1")<<endl;
    cout<<kv_parser.get("k2")<<endl;
    cout<<kv_parser.get("k3")<<endl;
    cout<<kv_parser.get("k4")<<endl;

    cout<<kv_parser.make_str().c_str()<<endl;

    kv_parser.set("k3", "v3");
    cout<<kv_parser.get("k3")<<endl;

    kv_parser.set("k5", "v5");
    cout<<kv_parser.get("k5")<<endl;

    cout<<kv_parser.make_str().c_str()<<endl;

    kv_parser.clear();

	return 0;
}
