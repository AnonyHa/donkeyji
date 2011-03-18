#ifndef _BUFFER_H
#define _BUFFER_H
#include <sm.h>

typedef struct _buffer {
	char* ptr;
	size_t size;
	size_t used;
	size_t offset;
} buffer;

buffer* buffer_new();
void buffer_free(buffer* b);
void buffer_reset(buffer* b);

//append Êý¾Ý
int buffer_append(buffer* b, char* buf, size_t size);
int buffer_movehead(buffer* b);

#endif
