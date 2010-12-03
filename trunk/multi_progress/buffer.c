#include <mp.h>
#include "buffer.h"

buffer* buffer_new()
{
	buffer* b = (buffer*)malloc(sizeof(buffer));
	if (b == NULL)
		return NULL;
	b->ptr = NULL;
	b->used = 0;
	b->size = 0;
	return b;
}

void buffer_free(buffer* b)
{
	if (b == NULL)
		return;
	free(b->ptr);
	free(b);	
}

//清空数据，但是不释放内存ptr
void buffer_reset(buffer* b)
{
	if (b == NULL)
		return;
	//不是放ptr的内存空间
	if (b->size > 0) {
		b->ptr[0] = '\0';//字符串结尾
	}
	b->used = 0;
}

//append数据到buffer末尾
int buffer_append(buffer* b, char* buf, size_t size)
{
	if (b == NULL)
		return -1;
	if (b->size == 0) {
		b->size = size + (128 - size % 128);
		b->ptr = (char*)malloc(b->size);
		if (b->ptr == NULL)
			return -1;
	} else if (b->size - b->used < size) {
		int less = size - (b->size - b->used);
		b->size += less + (128 - less % 128);
		b->ptr = (char*)realloc(b->ptr, b->size);
		if (b->ptr == NULL)
			return -1;
	}
	memcpy((void*)(b->ptr + b->used), (void*)buf, size);
	return 0;
}
