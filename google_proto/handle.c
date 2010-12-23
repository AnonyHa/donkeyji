#include <map>
#include "pto_macro.h"
#include "item_mod.h"
#include "login_mod.h"
#include <assert.h>

std::map<int, void*> handler;

int reg_msg()
{
	handler.insert(std::make_pair(S_BUY_ITEM_ID, (void*)on_s_buy_item));
	handler.insert(std::make_pair(S_LOGIN_ID, (void*)on_s_login));
	return 0;
}

int check_reg()
{
	for (int i=0; i<max_proto_id; i++)
		assert(handler[i]);
}
