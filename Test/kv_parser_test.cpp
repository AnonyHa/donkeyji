#include <string>
#include <map>
#include <iostream>

#include <gtest/gtest.h>

#include "kv_parser.h"
using namespace std;

TEST(KVParserTest, Basic)
{
    const char* str = "k1=v1; k2 =  v2  ; k3 =v3; k4=v4; k3=v5;  ";

    kv_parser kv_parser(str);

	ASSERT_STREQ(kv_parser.get("k1"), "v1");
	ASSERT_STREQ(kv_parser.get("k2"), "v2");
	ASSERT_STREQ(kv_parser.get("k3"), "v5");
	ASSERT_STREQ(kv_parser.get("k4"), "v4");

	ASSERT_STREQ(kv_parser.make_str().c_str(), "k1=v1;k2=v2;k3=v5;k4=v4");


    kv_parser.set("k3", "v3");
	ASSERT_STREQ(kv_parser.get("k3"), "v3");

    kv_parser.set("k5", "v5");
	ASSERT_STREQ(kv_parser.get("k5"), "v5");

	ASSERT_STREQ(kv_parser.make_str().c_str(), "k1=v1;k2=v2;k3=v3;k4=v4;k5=v5");

    kv_parser.clear();

	ASSERT_TRUE(kv_parser.get("k5") == NULL);
	ASSERT_STREQ(kv_parser.make_str().c_str(), "");
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
