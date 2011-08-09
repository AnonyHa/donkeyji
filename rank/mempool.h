#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include <stdlib.h>

typedef struct _mempool{

}mempool;
//global 
void* mem_alloc(size_t size);

void mem_free(void* mem);

#endif
