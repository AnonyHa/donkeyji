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
	//ά������ʹ�õ�����
	chunk* head;
	chunk* tail;
	//ά����unused����
	chunk* unused;
	size_t unused_chunks;
}chunkqueue;

chunk* chunk_new();
void chunk_free(chunk* c);

chunkqueue* chunkqueue_new();
void chunkqueue_free(chunkqueue* cq);
chunk* chunkqueue_get_append_chunk(chunkqueue* cq);//����/�����е�chunk
void chunkqueue_remove_finished_chunks(chunkqueue* cq);

#endif
