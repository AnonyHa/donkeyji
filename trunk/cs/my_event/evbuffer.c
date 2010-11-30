#include "event.h"
#include "evutil.h"

void bufferevent_read_pressure_cb(struct evbuffer* buf, size_t old, size_t now, void* arg);

//--------------
//non-interface
//--------------
static int bufferevent_add(struct event* ev, int timeout)
{
	struct ev_timeval tv;
	struct ev_timeval* ptv = NULL;

	if (timeout) {
		event_debugx("bufferevent_add timeout");
		evutil_timerclear(&tv);
		tv.tv_sec = timeout;
		ptv = &tv; 
	}
	return event_add(ev, ptv);//设置超时
}

static void bufferevent_readcb(int fd, short event, void* arg)
{
	event_debugx("bufferevent_readcb");
	struct bufferevent* bufev = arg;//之前保存的回调函数参数
	int res = 0;
	short what = EVBUFFER_READ;
	size_t len;
	int howmuch = -1;

	//若读写设置了超时，在process_timeout里会将event.ev_res设置为EV_TIMEOUT
	if (event == EV_TIMEOUT) {
		what |= EVBUFFER_TIMEOUT;
		goto error;//超时当初错误
	}

	//howmuch表示该缓冲区最多还能容纳的数据大小
	if (bufev->wm_read.high != 0) {
		howmuch = bufev->wm_read.high - EVBUFFER_LENGTH(bufev->input);
		if (howmuch <= 0) {
			struct evbuffer* buf = bufev->input;
			event_del(&bufev->ev_read);//为何要del呢????
			/*
			 * 缓冲区不够的处理
			 */
			evbuffer_setcb(buf, bufferevent_read_pressure_cb, bufev);
			return;
		}
	}

	res = evbuffer_read(bufev->input, fd, howmuch);
	if (res == -1) {//1.缓冲区expand错误，2.recv错误
		if (errno == EAGAIN || errno == EINTR) goto reschedule;//由于信号或者
		what |= EVBUFFER_ERROR;
	} else if (res == 0) {//recv返回0
		what |= EVBUFFER_EOF;
	}

	//res == 0: 对方主动断开
	if (res <= 0) {
		goto  error;//调用错误回调函数
	}

	//在process_active中被event_del，此处重新加入到event队列中
	bufferevent_add(&bufev->ev_read, bufev->timeout_read);

	/*
	 *  缓冲区处理 ??????
	 */
	len = EVBUFFER_LENGTH(bufev->input);//当前的有效数据长度
	if (bufev->wm_read.low != 0 && len < bufev->wm_read.low) return;//不调用回调
	if (bufev->wm_read.high !=0 && len >= bufev->wm_write.high) {//仍会调用回调
		struct evbuffer* buf = bufev->input;
		event_del(&bufev->ev_read);//不再处理该read的event
		evbuffer_setcb(buf, bufferevent_read_pressure_cb, bufev);//????
	}

	if (bufev->readcb != NULL) (*bufev->readcb)(bufev, bufev->cbarg);//cbarg: client连接的index，不是必须的
	return;

reschedule:
	bufferevent_add(&bufev->ev_read, bufev->timeout_read);//重新加入到
	return;
error:
	event_debugx("bufferevent_readcb goto error");
	if (bufev->errorcb != NULL)
		(*bufev->errorcb)(bufev, what, bufev->cbarg);//cbarg: vfd 调用bufferevent_free
}

//对于连接fd，如何触发EPOLLOUT的????
static void bufferevent_writecb(int fd, short event, void* arg)
{
	//event_debugx("bufferevent_writecb");
	struct bufferevent* bufev = arg;//之前保存的回调函数参数
	int res = 0;
	short what = EVBUFFER_WRITE;

	if (event == EV_TIMEOUT) {
		what |= EVBUFFER_TIMEOUT;
		goto error;//超时当成错误
	}

	//通过evbuffer_write写到bufev->output中
	if (EVBUFFER_LENGTH(bufev->output)) {//有数据?????为何加此判断
		res = evbuffer_write(bufev->output, fd);
		if (res == -1) {
			if (errno == EAGAIN || errno == EINTR || errno == EINPROGRESS) goto reschedule;
		} else if (res == 0) {
			what |= EVBUFFER_EOF;
		}
		if (res <= 0) goto error;//0为何也处理成错误?????
	}

	if (EVBUFFER_LENGTH(bufev->output) != 0) bufferevent_add(&bufev->ev_write, bufev->timeout_write);

	if (bufev->writecb != NULL) (*bufev->writecb)(bufev, bufev->cbarg);
	return;

reschedule:
	if (EVBUFFER_LENGTH(bufev->output) != 0) bufferevent_add(&bufev->ev_write, bufev->timeout_write);
	return;
error:
	//event_debugx("bufferevent_writecb goto error");
	(*bufev->errorcb)(bufev, what, bufev->cbarg);//调用bufferevent_free
}

//--------------------------------
//interface for outside revoltion
//--------------------------------

// cbarg: client连接的index，即vfd
struct bufferevent* bufferevent_new(int fd, evbuffercb readcb, evbuffercb writecb, everrorcb errorcb, void* cbarg)
{
	struct bufferevent* bufev;

	bufev = calloc(1, sizeof(struct bufferevent));
	if (bufev == NULL) return NULL;

	bufev->input = evbuffer_new();
	if (bufev->input == NULL) {
		event_warn("evbuffer_new failed");
		free(bufev);
		return NULL;
	}

	bufev->output = evbuffer_new();
	if (bufev->output == NULL) {
		event_warn("evbuffer_new failed");
		free(bufev);
		return NULL;
	}

	//对于客户连接fd，不用EV_PERSIST
	//在process_active时会从active_queue和epoll中删除该event
	//在bufferevent_readcb/bufferevent_writecb中会重新调用bufferevent_add到epoll中去
	//之所以会这样，是因为ev_read/ev_write需要重新设置ev_timeout的值，因为读写event是设置了超时的
	event_set(&bufev->ev_read, fd, EV_READ, bufferevent_readcb, bufev);
	event_set(&bufev->ev_write, fd, EV_WRITE, bufferevent_writecb, bufev);

	bufferevent_setcb(bufev, readcb, writecb, errorcb, cbarg);

	bufev->enabled = EV_WRITE;// ?????

	return bufev;
}

void bufferevent_free(struct bufferevent* bufev)
{
	event_debugx("bufferevent_free");
	//删除event
	event_del(&bufev->ev_read);
	event_del(&bufev->ev_write);

	evbuffer_free(bufev->input);
	evbuffer_free(bufev->output);

	//释放bufferevent内存，包含2个event的内存
	//event_del只是将event的指针从queue中删除，并不释放内存
	free(bufev);
}


void bufferevent_setcb(struct bufferevent* bufev, evbuffercb readcb, evbuffercb writecb, everrorcb errorcb, void* cbarg)
{
	bufev->readcb = readcb;
	bufev->writecb = writecb;
	bufev->errorcb = errorcb;
	bufev->cbarg = cbarg;
}

//有选择的激活ev_read/ev_write这个2个event
int bufferevent_enable(struct bufferevent* bufev, short event)
{
	if (event & EV_READ) {
		if (bufferevent_add(&bufev->ev_read, bufev->timeout_read) == -1) return -1;
	}

	if (event & EV_WRITE) {
		if (bufferevent_add(&bufev->ev_write, bufev->timeout_read) == -1) return -1;
	}

	bufev->enabled |= event;//修改激活标志位
	return 0;
}

void bufferevent_setwatermark(struct bufferevent* bufev, short events, size_t lowmark, size_t highmark)
{
	if (events & EV_READ) {
		bufev->wm_read.low = lowmark;
		bufev->wm_read.high = highmark;
	}
	if (events & EV_WRITE) {
		bufev->wm_write.low = lowmark;
		bufev->wm_write.high = highmark; 
	}

	//??????
	//watermark改变时，是否需要重新read一次
	bufferevent_read_pressure_cb(bufev->input, 0, EVBUFFER_LENGTH(bufev->input), bufev);
}

void bufferevent_read_pressure_cb(struct evbuffer* buf, size_t old, size_t now, void* arg)
{
	event_debugx("bufferevent_read_pressure_cb");
	struct bufferevent* bufev = arg;
	if (bufev->wm_read.high == 0 || now < bufev->wm_read.high) {
		evbuffer_setcb(buf, NULL, NULL);
		if (bufev->enabled & EV_READ) bufferevent_add(&bufev->ev_read, bufev->timeout_read);
	}
}

int bufferevent_write(struct bufferevent* bufev, const void* data, size_t size)
{
	int res;
	res = evbuffer_add(bufev->output, data, size);
	if (res == -1) return res;

	//为何此处需要再次bufferevent_add呢????
	if (size > 0 && (bufev->enabled & EV_WRITE)) {
		bufferevent_add(&bufev->ev_write, bufev->timeout_write);
	}
	return res;
}

int bufferevent_read(struct bufferevent* bufev, void* data, size_t size)
{
	struct evbuffer* buf = bufev->input;

	if (buf->off < size)
		size = buf->off;

	memcpy(data, buf->buffer, size);
	return size;
}
