#ifndef _BUFFER_H
#define _BUFFER_H

typedef struct _buffer
{
	char* ptr;
	size_t size;
	size_t used;
}buffer;

buffer* buffer_new();
void buffer_free(buffer* b);
void buffer_reset(buffer* b);

int buffer_append(buffer* b, char* buf, size_t size); 

#endif
