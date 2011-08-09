#include "mempool.h"
#include "stdlib.h"


int mempool_init()
{
	return 0;
}

//global
void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_free(void* mem)
{
	return free(mem);
}
