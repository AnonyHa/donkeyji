#include <mp.h>

#include "chunk.h"
#include "buffer.h"

static chunk* chunkqueue_get_unused_chunk(chunkqueue* cq);
static void chunkqueue_append_chunk(chunkqueue* cq, chunk* c);

chunk* 
chunk_new()
{
	chunk* c = (chunk*)malloc(sizeof(chunk));
	if (c == NULL)
		return NULL;

	c->mem = buffer_new();
	c->next = NULL;
	c->offset = 0;//没有被读过
}

void 
chunk_free(chunk* c)
{
	if (c == NULL)
		return;
	buffer_free(c->mem);
	//不free c->next
	free(c);
}

void 
chunk_reset(chunk* c)
{
	if (c == NULL)
		return;

	c->offset = 0;//标示为一个字节也没有读取过

	buffer_reset(c->mem);

	//不处理c->next
}

//---------------------------------------
chunkqueue* 
chunkqueue_new()
{
	chunkqueue* cq = (chunkqueue*)malloc(sizeof(chunkqueue));
	if (cq == NULL)
		return NULL;
	cq->head = NULL;
	cq->tail = NULL;
	cq->unused = NULL;
	cq->unused_chunks = 0;
}

void 
chunkqueue_free(chunkqueue* cq)
{
	chunk* c;
	chunk* tmp;

	if (cq == NULL)
		return;

	for (c=cq->head; c!=NULL; ) {
		tmp = c->next;
		chunk_free(c);
		c = tmp;
	}

	for (c=cq->unused; c!=NULL; c=c->next) {
		tmp = c->next;
		chunk_free(c);
		c = tmp;
	}

	free(cq);
}

void 
chunkqueue_reset(chunkqueue* cq)
{
	if (cq == NULL)
		return;

	chunk* c;
	chunk* tmp;

	
	/*
	 * 标示为全部已经读取过了 c->offset = c->mem->used-1
	 * 以便在chunkqueue_remove_finished_chunks中把处理完了的chunk放入unused链表
	 */
	for (c=cq->head; c!=NULL;) {
		c->offset = c->mem->used-1;	
		c = c->next;
	}

	chunkqueue_remove_finished_chunks(cq);
}

//对外接口
chunk* 
chunkqueue_get_append_chunk(chunkqueue* cq)
{
	//从unused链表中找到一个chunk
	chunk* c = chunkqueue_get_unused_chunk(cq);
	if (c == NULL)
		return NULL;
	//append放入使用链表中
	chunkqueue_append_chunk(cq, c);//append 到queue中
	return c; 
}

static chunk* 
chunkqueue_get_unused_chunk(chunkqueue* cq)
{
	chunk* c;

	if (cq->unused == NULL) {
		c = chunk_new();//不放入unused链表，因为该chunk是要使用的
	} else {
		c = cq->unused;//取下unused链表的表头使用
		cq->unused = c->next;
		c->next = NULL;//该chunk与unused链表脱离关系
		cq->unused_chunks--;
	}

	return c;
}

//只管append，不管unused变化
static void 
chunkqueue_append_chunk(chunkqueue* cq, chunk* c)
{
	/* 无需做这种检查，相信调用者
	if (cq == NULL)
		return;
	*/
	if (cq->tail != NULL)
		cq->tail->next = c;
	cq->tail = c;
	if (cq->head == NULL)
		cq->head = c;
}

//把已经处理完了的chunk放入unused链表
void 
chunkqueue_remove_finished_chunks(chunkqueue* cq)
{
	chunk* c;
	int is_finished = 0;

	for (c=cq->head; c!=NULL; c=cq->head) {
		//chunk为空，或者chunk已经读取完了
		if (c->mem->used == 0 || c->offset == c->mem->used-1)
			is_finished = 1;

		if (is_finished == 0)
			break;

		chunk_reset(c);//c->offset = 0

		cq->head = c->next;
		if (c == cq->tail)
			cq->tail = NULL;

		if (cq->unused_chunks >= 4) {
			chunk_free(c);
		} else {
			c->next = cq->unused;
			cq->unused = c;
			cq->unused_chunks++;
		}
	}
}
