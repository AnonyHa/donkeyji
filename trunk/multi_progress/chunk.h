#ifndef _CHUNK_H
#define _CHUNK_H

#include "buffer.h"

typedef struct _chunk
{
	buffer* mem;
	struct _chunk* next;
}chunk;

typedef struct _chunkqueue
{
	chunk* head;
	chunk* tail;
	chunk* unused;
	size_t unused_chunks;
}chunkqueue;

chunk* chunk_new();
void chunk_free(chunk* c);

chunkqueue* chunkqueue_new();
void chunkqueue_free(chunkqueue* cq);
chunk* chunkqueue_get_append_chunk();//新增/用已有的chunk

#endif
