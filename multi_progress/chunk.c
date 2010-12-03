#include "chunk.h"
//#include "buffer.h"
static chunk* chunkqueue_get_unused_chunk(chunkqueue* cq);
static void chunkqueue_append_chunk(chunkqueue* cq, chunk* c);

chunk* chunk_new()
{
	chunk* c = (chunk*)malloc(sizeof(chunk));
	if (c == NULL)
		return NULL;

	c->mem = buffer_new();
	c->next = NULL;
}

void chunk_free(chunk* c)
{
	if (c == NULL)
		return;
	buffer_free(c->mem);
	//不处理c->next
	free(c);
}

void chunk_reset(chunk* c)
{
	if (c == NULL)
		return;
	buffer_reset(c->mem);
	//不处理c->next
}

//---------------------------------------
chunkqueue* chunkqueue_new()
{
	chunkqueue* cq = (chunkqueue*)malloc(sizeof(chunkqueue));
	if (cq == NULL)
		return NULL;
	cq->head = NULL;
	cq->tail = NULL;
	cq->unused = NULL;
	cq->unused_chunks = 0;
}

void chunkqueue_free(chunkqueue* cq)
{}

//对外接口
chunk* chunkqueue_get_append_chunk(chunkqueue* cq)
{
	chunk* c = chunkqueue_get_unused_chunk(cq);
	if (c == NULL) {
		return NULL;
	}
	chunkqueue_append_chunk(cq, c);//append 到queue中
	return c; 
}

static chunk* chunkqueue_get_unused_chunk(chunkqueue* cq)
{
	chunk* c;
	if (cq->unused == NULL) {
		c = chunk_new();
	} else {
		c = cq->unused;
		cq->unused = c->next;
		c->next = NULL;//??为何要赋NULL
		cq->unused_chunks--;
	}
	return c;
}

//只管append，不管unused变化
static void chunkqueue_append_chunk(chunkqueue* cq, chunk* c)
{
	/* 无需做这种检查，相信调用者
	if (cq == NULL)
		return;
	*/
	if (c->tail != NULL)
		cq->tail->next = c;
	cq->tail = c;
	if (c->head == NULL)
		c->head = c;
}
