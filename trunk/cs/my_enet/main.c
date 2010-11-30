#include "enet/enet.h"

int main()
{
	ENetAddress ea;
	enet_address_set_host(&ea, "127.0.0.1", 3000);
	return 0;
}
