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

//������ݣ����ǲ��ͷ��ڴ�ptr
void buffer_reset(buffer* b)
{
	if (b == NULL)
		return;
	//���Ƿ�ptr���ڴ�ռ�
	if (b->size > 0) {
		b->ptr[0] = '\0';//�ַ�����β
	}
	b->used = 0;
}

//append���ݵ�bufferĩβ
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
