#include <mp.h>

#include "server.h"
#include "conn.h"
#include "log.h"
#include "chunk.h"

static void conn_process_chunk(conn* c);
static void _conn_handle_read(struct bufferevent* bev, void* arg);
static void _conn_handle_err(struct bufferevent* bev, short what, void* arg);

//---------------------------------------------------

static conn* 
conn_new()
{
	conn* c = (conn*)calloc(1, sizeof(conn));
	assert(c);
	c->fd = -1;//����Ҳ������fd
	c->ndx = -1;//��ʾ��û�з���conn_mgr

	c->bev = NULL;//���ﲻע���¼�

	c->read_q = chunkqueue_new();
	c->write_q = chunkqueue_new();

	return c;
}

static void
conn_free(conn* c)
{
	if (c == NULL)
		return;

	if (c->bev != NULL)
		bufferevent_free(c->bev);

	if (c->read_q != NULL)
		chunkqueue_free(c->read_q);

	if (c->write_q != NULL)
		chunkqueue_free(c->write_q);

	close(c->fd);

	free(c);
}

//Ϊconnע��read/write�¼�
static void
conn_register_event(conn* c)
{
	//ֻ����������bufferevent
	c->bev = bufferevent_new(c->fd, _conn_handle_read, NULL, _conn_handle_err, (void*)c);
	//log_msg(__FILE__, __LINE__, "bufferevent_new c->bev->input = %x", c->bev->input);
	//log_msg(__FILE__, __LINE__, "bufferevent_new c->bev->output = %x", c->bev->output);
	if (c->bev == NULL) {
		log_msg(__FILE__, __LINE__, "bufferevent_new failed");
		return;
	}
	bufferevent_enable(c->bev, EV_READ|EV_WRITE);
}

//ɾ��Ϊconnע����¼�
static void 
conn_unregister_event(conn* c)
{
	bufferevent_free(c->bev);
}

//----------------------
//���ã��������ͷ��ڴ�
//----------------------
static void
conn_reset(conn* c)
{
	if (c == NULL)
		return;

	//buffervern������reset��ֻ��free����ע���ʱ����malloc�ˣ�����...
	conn_unregister_event(c);

	if (c->read_q != NULL)
		chunkqueue_reset(c->read_q);

	if (c->write_q != NULL)
		chunkqueue_reset(c->write_q);

	close(c->fd);

	c->fd = -1;		
	c->ndx = -1;
}

//---------------------------------------------------
conn_mgr* 
conn_mgr_new()
{
	conn_mgr* cm = (conn_mgr*)calloc(1, sizeof(conn_mgr));
	assert(cm);
	cm->size = 0;
	cm->used = 0;
	cm->ptr = NULL;
	return cm;
}

void 
conn_mgr_free(conn_mgr* cm)
{
	int i;
	if (cm == NULL)
		return;

	if (cm->ptr != NULL) {
		for (i=0; i<cm->used; i++) {
			conn_free(cm->ptr[i]);
		}
		free(cm->ptr);
	}

	free(cm);
}

//��Ԥ�ȷ��䣬��128���ٶ�����
void 
conn_mgr_add_conn(conn_mgr* cm, int sock)
{
	conn* c;
	int i;
	log_msg(__FILE__, __LINE__, "5555555");
	if (cm->size == 0) {//��һ�η���
		cm->size = 128;
		log_msg(__FILE__, __LINE__, "6666666");
		cm->ptr = (conn**)calloc(cm->size, sizeof(conn*));
		log_msg(__FILE__, __LINE__, "7777777");
		for (i=0; i<cm->size; i++) {
			cm->ptr[i] = conn_new();
		}
	} else if (cm->size == cm->used) {//֮ǰ�����������
		cm->size += 128;	
		cm->ptr = (conn**)realloc(cm->ptr, cm->size);
		for (i=cm->used; i<cm->size; i++) {
			cm->ptr[i] = conn_new();
		}
	}

	c = cm->ptr[cm->used];
	//conn_reset(c);//��ʵ�ڹ黹conn����ʱ�Ѿ�reset���ˣ����������ظ�

	c->ndx = cm->used;
	cm->used++;

	//ע�������¼�
	c->fd = sock;
	conn_register_event(c);
}

//
static void 
conn_mgr_del_conn(conn_mgr* cm, conn* c)
{
	int ndx;
	conn* tmp;
	if (c == NULL)
		return;

	ndx = c->ndx;
	if (ndx == -1)//����������
		return;

	if (ndx == cm->used - 1) {//������β��
		c->ndx = -1;
		conn_reset(c);//�Ա��´�ʹ��
		return;
	}

	//��������β��Ҫɾ�����Ǹ�conn
	tmp = c;
	cm->ptr[ndx] = cm->ptr[cm->used-1];
	cm->ptr[ndx]->ndx = ndx;
	cm->ptr[cm->used-1] = tmp;
	cm->ptr[cm->used-1]->ndx = -1;//��ʾΪδʹ��

	//ɾ��read/write�¼�����sock = -1
	conn_reset(c);

	return;
}

//----------------------------------------------------------
static void 
_conn_handle_read(struct bufferevent* bev, void* arg)
{
	log_msg(__FILE__, __LINE__, "data from conn to read: fd = %d", ((conn*)arg)->fd);
	log_msg(__FILE__, __LINE__, "bev->input = %d", bev->input);

	/* ��֣�Ϊʲôbev->inputʼ��Ϊ0????
	size_t len = bev->input->off;
	if (len == 0) {
		log_msg(__FILE__, __LINE__, "no data");
		return;
	}
	*/

	//�㹻��Ļ���
	conn* c = (conn*)arg;//connection����
	size_t len = 1024;
	char buf[len];
	bzero(buf, len);

	size_t read_len = bufferevent_read(bev, buf, len);
	if (read_len <= 0) {
		log_msg(__FILE__, __LINE__, "bufferevent read error: %s", strerror(errno));
		return;
	}
	log_msg(__FILE__, __LINE__, "bufferevent read len = %d", read_len);

	chunk* ck = chunkqueue_get_append_chunk(c->read_q);	
	if (ck == NULL) 
		return;
	buffer_append(ck->mem, buf, read_len);
	log_msg(__FILE__, __LINE__, "buffer used = %d, size = %d", ck->mem->used, ck->mem->size);

	log_msg(__FILE__, __LINE__, "bev = %x", bev);
	log_msg(__FILE__, __LINE__, "input = %x", bev->input);
	log_msg(__FILE__, __LINE__, "output = %x", bev->output);
	log_msg(__FILE__, __LINE__, "read data: %s", buf);

	//ÿ����readʱ���ͽ����Ž���һ�����ݴ���
	conn_process_chunk(c);
}


static void 
_conn_handle_err(struct bufferevent* bev, short what, void* arg)
{
	conn* c = (conn*)arg;//connection����
	log_msg(__FILE__, __LINE__, "conn error: fd = %d", c->fd);
	//---------------
	//use global srv
	//---------------
	conn_mgr_del_conn(srv->conns, c);
	log_msg(__FILE__, __LINE__, "close sock: Fd = %d", c->fd);
}

//---------------------------------------
//ÿ����readʱ���ͽ����Ž���һ�����ݴ���
//---------------------------------------
static void 
conn_process_chunk(conn* c)
{}
