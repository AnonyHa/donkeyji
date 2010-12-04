#ifndef _CHUNK_H
#define _CHUNK_H

#include "buffer.h"

typedef struct _chunk
{
	buffer* mem;
	off_t offset;//处理过的字节
	struct _chunk* next;
}chunk;

typedef struct _chunkqueue
{
	//维护的在使用的链表
	chunk* head;
	chunk* tail;
	//维护的unused链表
	chunk* unused;
	size_t unused_chunks;
}chunkqueue;

chunk* chunk_new();
void chunk_free(chunk* c);
void chunk_reset(chunk* c);

chunkqueue* chunkqueue_new();
void chunkqueue_free(chunkqueue* cq);
void chunkqueue_reset(chunkqueue* cq);
chunk* chunkqueue_get_append_chunk(chunkqueue* cq);//新增/用已有的chunk
void chunkqueue_remove_finished_chunks(chunkqueue* cq);

#endif
