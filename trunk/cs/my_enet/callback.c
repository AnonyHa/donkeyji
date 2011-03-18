#include <stdlib.h>
#include "enet/enet.h"

void* enet_malloc(size_t size)
{
	void* memory = malloc(size);

	if (memory == NULL) {
		abort();
	}

	return memory;
}

void enet_free(void* memory)
{
	free(memory);
}

int enet_rand(void)
{
	return rand();
}
