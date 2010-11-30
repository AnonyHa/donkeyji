#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "event.h"
#include "evutil.h"


struct evbuffer* evbuffer_new(void)
{
	struct evbuffer* buf;
	buf = calloc(1, sizeof(struct evbuffer));
	return buf;
}

void evbuffer_free(struct evbuffer* buf)
{
	if (buf->orig_buffer != NULL)
		free(buf->orig_buffer);
	free(buf);
}

//从buf->buffer开始消耗
void evbuffer_drain(struct evbuffer* buf, size_t len)
{
	size_t oldoff = buf->off;

	if (len >= buf->off) {//一下子全部消耗完
		buf->off = 0;
		buf->buffer = buf->orig_buffer;
		buf->misalign = 0; 
		goto done;
	}

	buf->buffer += len;
	buf->misalign += len;
	buf->off -= len;
done:
	if (buf->off != oldoff && buf->cb != NULL) {
		event_debugx("evbuffer callback");
		(*buf->cb)(buf, oldoff, buf->off, buf->cbarg); 
	}
}

#define EVBUFFER_MAX_READ 4096

//从给定的fd上recv数据
int evbuffer_read(struct evbuffer* buf, int fd, int howmuch)
{
	u_char* p;
	size_t oldoff = buf->off;
	int n = EVBUFFER_MAX_READ;

	//设置接收缓冲区的字节数
	if (ioctl(fd, FIONREAD, &n) == -1 || n == 0) {
		n = EVBUFFER_MAX_READ;
	} else if (n > EVBUFFER_MAX_READ && n > howmuch) {}

	/*
	 * 缓冲区处理
	 */
	if (howmuch < 0 || howmuch > n) howmuch = n;

	if (evbuffer_expand(buf, howmuch) == -1) return -1;

	p = buf->buffer + buf->off;//get buf
	n = recv(fd, p, howmuch, 0);//执行recv操作
	if (n == -1) return -1;
	if (n == 0) return 0;

	/*
	 * 缓冲区处理
	 */
	buf->off += n;
	// 缓冲区的回调函数
	if (buf->off != oldoff && buf->cb != NULL) (*buf->cb)(buf, oldoff, buf->off, buf->cbarg);

	return n;
}

//把缓冲区的数据send到指定的fd上去
int evbuffer_write(struct evbuffer* buf, int fd)
{
	event_debugx("evbuffer_write");
	int n;
	event_debugx("evbuffer_write: before send");
	//如果buf->off为0，会出错
	n = send(fd, buf->buffer, buf->off, 0);
	event_debugx("evbuffer_write: after send");
	if (n == -1) return -1;
	if (n == 0) return 0;
	evbuffer_drain(buf, n);//掉了这句导致段错误
	event_debugx("send len = %d", n);	
	return n;
}

//内部接口
//将有效有效数据移动到缓冲区地址的首部
static void evbuffer_align(struct evbuffer* buf)
{
	memmove(buf->orig_buffer, buf->buffer, buf->off);
	buf->buffer = buf->orig_buffer;
	buf->misalign = 0;//偏移置为0
}

// datlen: 需要再加的长度
int evbuffer_expand(struct evbuffer* buf, size_t datlen)
{
	size_t need = buf->misalign + buf->off + datlen;//需要的总长度
	if (buf->totallen >= need) return 0;//无需扩展，空间已经足够

	if (buf->misalign >= datlen) {//空闲出的位置已经足够了
		evbuffer_align(buf);
	} else {
		void* newbuf;
		size_t length = buf->totallen;
		if (length < 256) length = 256;

		//以256的翻倍
		while (length < need) length <<= 1;// * 2

		if (buf->orig_buffer != buf->buffer) evbuffer_align(buf);//移到头
		if ((newbuf = realloc(buf->buffer, length)) == NULL) return -1;//扩展空间
		buf->orig_buffer = buf->buffer = newbuf;
		buf->totallen = length;
	}
	return 0;//表示成功
}

void evbuffer_setcb(struct evbuffer* buf, void(*cb)(struct evbuffer*, size_t, size_t, void*), void* cbarg)
{}

int evbuffer_add(struct evbuffer* buf, const void* data, size_t datlen)
{
	size_t need = buf->misalign + buf->off + datlen;
	size_t oldoff = buf->off;

	if (buf->totallen < need) {
		if (evbuffer_expand(buf, datlen) == -1) return -1;
	}

	memcpy(buf->buffer + buf->off, data, datlen);
	buf->off += datlen;

	if (datlen && buf->cb != NULL) {
		(*buf->cb)(buf, oldoff, buf->off, buf->cbarg);
	}
	return 0;
}
