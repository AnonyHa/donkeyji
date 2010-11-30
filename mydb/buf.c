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
#define CHKSZ()			CHKPAYLOADSZ(HEAD)//获取client数据包的包长
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
	/** 会多出一个字节 char buf[1] */
	db_buf * mbuf = (db_buf*) malloc (total + sizeof(db_buf)) ;//db_buf自身的大小和它管理的缓冲区大小---
	mbuf->total = total ;
	db_buf_Clear (mbuf) ;// TAIL = HEAD = START

	return mbuf ;
}

void 
db_buf_delete (db_buf *mbuf) 
{
	free (mbuf) ;//释放缓冲区---
}


//把数据向START移动，保证空间再利用---
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
	if (LEFT () < len) //剩余空间不够用，就向START移动，腾出空间---
	{
		db_buf_Move (mbuf) ;
	}

	// 如果还是空间不够，则返回NULL---
	return (LEFT () >= len) ? TAIL : NULL ; //返回TAIL指针---
}

/** 封包pack 专用 */
void * db_buf_alloc_pack (db_buf *mbuf, size_t len)
{
	//static int  SetAside = LOCALHDRSZ + CHKHDRSZ ;
	//char * p = (char *)db_buf_alloc (mbuf, len + SetAside) ;
	//return (p != NULL) ? (p + SetAside) : NULL ;
}

/** vfd buffer 专用 */
INTERNAL void *
db_buf_Chunk (db_buf *mbuf, size_t *len)
{
	size_t sz = SIZE () ;//当前的有效数据长度---

	// 当前有效数据大于一个包头长 && client数据包的有效参数pack后的长度 + client数据包头长度 < sz
	/*if ((CHKHDRSZ <= sz) && ((CHKSZ () + CHKHDRSZ) <= sz)) 
	{
		if (NULL != len) 
		{
			//*len = CHKSZ () ;//真正的client的package长度---
		}
		//return SEEKHEAD (CHKHDRSZ) ;//返回client包的开始位置---
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

