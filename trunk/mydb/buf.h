#ifndef _BUF_H
#define _BUF_H

struct db_buf {
	char *h;
	char *t;
	size_t total;
	char buf[1];
};


#define db_buf_head(mbuf)		((mbuf)->h)
#define db_buf_tail(mbuf)		((mbuf)->t)
#define db_buf_total(mbuf)		((mbuf)->total)
#define db_buf_first(mbuf)		((mbuf)->buf)
#define db_buf_size(mbuf)		((size_t)((mbuf)->t - (mbuf)->h))
#define db_buf_left(mbuf)		((size_t)((mbuf)->total + (mbuf)->buf - (mbuf)->t))

#define db_buf_need_flush(mbuf)	(db_buf_size(mbuf) > cfgData.FlushBase)

//struct db_buf* db_buf_new (size_t total) ;
void db_buf_delete (struct db_buf *mbuf) ;
void * db_buf_alloc (struct db_buf *mbuf, size_t  len) ;
void * db_buf_alloc_pack (struct db_buf *mbuf, size_t len) ;
void * db_buf_seek_head (struct db_buf *mbuf, size_t len) ;
void * db_buf_seek_tail (struct db_buf *mbuf, size_t len) ;
void db_buf_full (struct db_buf *mbuf) ;

#endif
