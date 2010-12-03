#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "buffer.h"

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
	//������c->next
	free(c);
}

void chunk_reset(chunk* c)
{
	if (c == NULL)
		return;
	buffer_reset(c->mem);
	//������c->next
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
}

//����ӿ�
chunk* chunkqueue_get_append_chunk(chunkqueue* cq)
{
	chunk* c = chunkqueue_get_unused_chunk(cq);//��unused�������ҵ�һ��chunk
	if (c == NULL)
		return NULL;
	chunkqueue_append_chunk(cq, c);//append ��queue��
	return c; 
}

static chunk* chunkqueue_get_unused_chunk(chunkqueue* cq)
{
	chunk* c;
	if (cq->unused == NULL) {
		c = chunk_new();//������unused������Ϊ��chunk��Ҫʹ�õ�
	} else {
		c = cq->unused;//ȡ��unused����ı�ͷʹ��
		cq->unused = c->next;
		c->next = NULL;//��chunk��unused���������ϵ
		cq->unused_chunks--;
	}
	return c;
}

//ֻ��append������unused�仯
static void chunkqueue_append_chunk(chunkqueue* cq, chunk* c)
{
	/* ���������ּ�飬���ŵ�����
	if (cq == NULL)
		return;
	*/
	if (cq->tail != NULL)
		cq->tail->next = c;
	cq->tail = c;
	if (cq->head == NULL)
		cq->head = c;
}

void chunkqueue_remove_finished_chunks(chunkqueue* cq)
{}
