#include "pto_macro.h"

//---------------------------
//send C_BUY_ITEM_T interface
//---------------------------
int send_C_BUY_ITEM_T(C_BUY_ITEM_T* obj)
{
	char buf[2048];
	unsigned int proto_id = C_BUY_ITEM_ID;
	int proto_len = sizeof(unsigned int);
	memcpy(buf, &proto_id, proto_len);
	memcpy(buf + proto_len, obj);
	return 0;
}

				
//---------------------------
//send C_LOGIN_T interface
//---------------------------
int send_C_LOGIN_T(C_LOGIN_T* obj)
{
	char buf[2048];
	unsigned int proto_id = C_LOGIN_ID;
	int proto_len = sizeof(unsigned int);
	memcpy(buf, &proto_id, proto_len);
	memcpy(buf + proto_len, obj);
	return 0;
}

				