#include "databuf.h"
#include <arpa/inet.h>

#include <assert.h>
#include <string.h>


DataBuf::DataBuf(int len)
{
	_len = len;
	_data = new char[_len];
	_head = _tail = 0;// ��ʾû������
	_left = _len - _tail;
}

DataBuf::~DataBuf()
{
	if (_data != NULL) {
		delete []_data;
	}
}
// ��ʼλ���ƶ�
void DataBuf::moveHead()
{
	memmove(_data, _data+_head, _tail-_head);// �ƶ�_tail-_head���ֽ�
	_tail = _tail - _head;
	_left = _left + _head;
	_head = 0;
}

// ���β����len���ֽ�
int DataBuf::appendRawData(byte* data, int len)
{
	/*
	if (len > _left)
		moveHead();
	if (len > _left)//�ռ���Ȼ����
		return -1;
	*/
	// �����µ��߼�
	expandBuf(len);
	memcpy(_data + _tail, data, len);
	_tail = _tail + len;
	_left = _left - len;
	return 0;
}

// ��ȥǰ���len���ֽ�
int DataBuf::cutData(int len)
{
	if (len > _tail - _head) {//���ݲ���
		cout<<"full"<<endl;
		return -1;
	}

	_head = _head + len;
	return 0;
}

// ����һ�����������ݰ����׵�ַ, ����һ�����������ͷ���NULL��0
char* DataBuf::getPack(short& size)
{
	int valid = _tail - _head;

	if (valid >= sizeof(short) && valid >= (_data[_head] + sizeof(short))) {
		size = (*(short*)(_data+_head));//ȡ�ð�ͷֵ
		return _data + _head + sizeof(short);//����ʼ�ĵ�ַ
	} else {//����һ���������ͷ���NULL
		size = 0;
		return NULL;
	}
}

// �������ÿռ䣬����Ч���ݳ���
int DataBuf::getDataSize()
{
	return _tail - _head;
}

// ����_headָ��ĵ�ַ
char* DataBuf::getSendData()
{
	return (_data + _head);
}

void DataBuf::printData()
{
	cout<<"_head = "<<_head<<endl;
	cout<<"_tail = "<<_tail<<endl;
	cout<<"_left = "<<_left<<endl;
	cout<<"_len = "<<_len<<endl;
}

// ������룬Ҫô�ɹ���Ҫô��ͷ����������
int DataBuf::appendPack(byte* data, int len)
{
	short head = len;
	short size = len + sizeof(short);

	if (size > _left) {
		moveHead();
	}

	if (size > _left) { //�ռ���Ȼ����
		return -1;    // ����ӣ�ֱ���˳�
	}

	memcpy(_data+_tail, (char*)&head, sizeof(short));
	_tail = _tail + sizeof(short);
	_left = _left - sizeof(short);

	memcpy(_data + _tail, data, len);
	_tail = _tail + len;
	_left = _left - len;
	return 0;
}

// �ȶ��������������жϣ���������չ
int DataBuf::expandBuf(int need)
{
	moveHead();//���Ƶ�ͷ

	if (_left >= need) { //��ʱ������ǿռ䲻����expand
		return 0;
	}

	int newLen = _len<<1;//����2��
	int realNeed = _len + need;//�ܹ���Ҫ�ĳ���

	while (newLen < realNeed) {
		newLen <<= 1;
	}

	char* newData = new char[newLen];

	if (newData == NULL) {
		return -1;
	}

	memcpy(newData, _data, _len);
	delete[] _data;
	// _head, _tail��ʱ����Ҫ�ı�
	_data = newData;
	_len = newLen;
	_left = _len - _tail;

	return 0;
}

