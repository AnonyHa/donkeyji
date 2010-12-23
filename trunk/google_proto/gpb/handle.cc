#include <map>
#include <assert.h>
#include "pto_macro.h"
#include "item_mod.h"
#include "login_mod.h"

std::map<int, void*> handler;
extern int is_svr[];

int reg_msg()
{
	handler.insert(std::make_pair(S_LOGIN_ID, (void*)on_s_login));
	handler.insert(std::make_pair(S_BUY_ITEM_ID, (void*)on_s_buy_item));
	return 0;
}

int check_reg()
{
	for (int i=1; i<MAX_PROTO_ID; i++) {
		printf("i = %d\n", i);
		if (is_svr[i])
			assert(handler[i]);
		printf("func %d: %x\n", i, handler[i]);
	}
	return 0;
}
