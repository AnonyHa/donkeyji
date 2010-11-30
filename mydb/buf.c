#include "afxdb.h"


/** data members of db_buf */
#define START			(mbuf->buf)
#define END			(mbuf->total + mbuf->buf)
#define HEAD			(mbuf->h)
#define TAIL			(mbuf->t)

/** member methods of db_buf */
#define LEFT()			((size_t)(END - TAIL))
#define SIZE()			((size_t)(TAIL - HEAD))
#define SEEKHEAD(len)		(HEAD += (len))
#define SEEKTAIL(len)		(TAIL += (len))
#define CHKSZ()			CHKPAYLOADSZ(HEAD)//��ȡclient���ݰ��İ���
/** be careful! set tail before head, or reset(SIZE()) runs wrong */
#define RESET(sz)		do {TAIL = START + (sz) ; HEAD = START ;} while (0)

#define INTERNAL	inline static  

INTERNAL void 
db_buf_Clear (db_buf *mbuf) 
{
	TAIL = HEAD = START ;
}

void 
db_buf_full (db_buf *mbuf) 
{
	HEAD = START ;
	TAIL = END ;
}

db_buf * 
db_buf_new (size_t total) 
{
	/** ����һ���ֽ� char buf[1] */
	db_buf * mbuf = (db_buf*) malloc (total + sizeof(db_buf)) ;//db_buf����Ĵ�С��������Ļ�������С---
	mbuf->total = total ;
	db_buf_Clear (mbuf) ;// TAIL = HEAD = START

	return mbuf ;
}

void 
db_buf_delete (db_buf *mbuf) 
{
	free (mbuf) ;//�ͷŻ�����---
}


//��������START�ƶ�����֤�ռ�������---
INTERNAL size_t 
db_buf_Move (db_buf *mbuf) 
{
	size_t sz = SIZE () ;
	memmove (START, HEAD, sz) ;
	RESET (sz) ;
	return sz ;
}


INTERNAL void * 
db_buf_AllocLess (db_buf *mbuf, size_t  len, size_t * rlen) 
{
	if (LEFT () < len) 
	{
		db_buf_Move (mbuf) ;
	}

	*rlen = (LEFT () >= len) ? len : LEFT () ; 
	return (0 < *rlen) ? TAIL : NULL ; 
}

void * 
db_buf_alloc (db_buf *mbuf, size_t  len) 
{
	if (LEFT () < len) //ʣ��ռ䲻���ã�����START�ƶ����ڳ��ռ�---
	{
		db_buf_Move (mbuf) ;
	}

	// ������ǿռ䲻�����򷵻�NULL---
	return (LEFT () >= len) ? TAIL : NULL ; //����TAILָ��---
}

/** ���pack ר�� */
void * db_buf_alloc_pack (db_buf *mbuf, size_t len)
{
	//static int  SetAside = LOCALHDRSZ + CHKHDRSZ ;
	//char * p = (char *)db_buf_alloc (mbuf, len + SetAside) ;
	//return (p != NULL) ? (p + SetAside) : NULL ;
}

/** vfd buffer ר�� */
INTERNAL void *
db_buf_Chunk (db_buf *mbuf, size_t *len)
{
	size_t sz = SIZE () ;//��ǰ����Ч���ݳ���---

	// ��ǰ��Ч���ݴ���һ����ͷ�� && client���ݰ�����Ч����pack��ĳ��� + client���ݰ�ͷ���� < sz
	/*if ((CHKHDRSZ <= sz) && ((CHKSZ () + CHKHDRSZ) <= sz)) 
	{
		if (NULL != len) 
		{
			//*len = CHKSZ () ;//������client��package����---
		}
		//return SEEKHEAD (CHKHDRSZ) ;//����client���Ŀ�ʼλ��---
	} 
	else 
	{
		if (NULL != len) 
		{ 
			*len = 0 ; 
		}
		return NULL ;
	}
	*/
}

void * 
db_buf_seek_tail (db_buf *mbuf, size_t len)
{
	return (len <= LEFT ()) ? SEEKTAIL (len) : NULL ; 
}

void * 
db_buf_seek_head (db_buf *mbuf, size_t len)
{
	if (len > SIZE ()) 
		return NULL ;
	char * h = SEEKHEAD (len) ; 
	if (HEAD == TAIL) /* if empty, rewind the buffer */
		db_buf_Clear (mbuf) ;

	return h ;
}

