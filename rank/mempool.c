#include "stdlib.h"

typedef struct _mempool{

}mempool;

int mempool_init()
{}

//global 
void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_free(void* mem)
{
	return free(mem);
}
