#include "databuf.h"
#include <arpa/inet.h>

#include <assert.h>
#include <string.h>


DataBuf::DataBuf(int len)
{
	_len = len;
	_data = new char[_len];
	_head = _tail = 0;// 表示没有数据
	_left = _len - _tail;
}

DataBuf::~DataBuf()
{
	if (_data != NULL) {
		delete []_data;
	}
}
// 向开始位置移动
void DataBuf::moveHead()
{
	memmove(_data, _data+_head, _tail-_head);// 移动_tail-_head个字节
	_tail = _tail - _head;
	_left = _left + _head;
	_head = 0;
}

// 向队尾增加len个字节
int DataBuf::appendRawData(byte* data, int len)
{
	/*
	if (len > _left)
		moveHead();
	if (len > _left)//空间仍然不够
		return -1;
	*/
	// 采用新的逻辑
	expandBuf(len);
	memcpy(_data + _tail, data, len);
	_tail = _tail + len;
	_left = _left - len;
	return 0;
}

// 截去前面的len个字节
int DataBuf::cutData(int len)
{
	if (len > _tail - _head) {//数据不够
		cout<<"full"<<endl;
		return -1;
	}

	_head = _head + len;
	return 0;
}

// 返回一个完整的数据包的首地址, 不够一个完整包，就返回NULL和0
char* DataBuf::getPack(short& size)
{
	int valid = _tail - _head;

	if (valid >= sizeof(short) && valid >= (_data[_head] + sizeof(short))) {
		size = (*(short*)(_data+_head));//取得包头值
		return _data + _head + sizeof(short);//包身开始的地址
	} else {//不够一个完整包就返回NULL
		size = 0;
		return NULL;
	}
}

// 返回已用空间，即有效数据长度
int DataBuf::getDataSize()
{
	return _tail - _head;
}

// 返回_head指向的地址
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

// 整体加入，要么成功，要么包头、包身都不加
int DataBuf::appendPack(byte* data, int len)
{
	short head = len;
	short size = len + sizeof(short);

	if (size > _left) {
		moveHead();
	}

	if (size > _left) { //空间仍然不够
		return -1;    // 不添加，直接退出
	}

	memcpy(_data+_tail, (char*)&head, sizeof(short));
	_tail = _tail + sizeof(short);
	_left = _left - sizeof(short);

	memcpy(_data + _tail, data, len);
	_tail = _tail + len;
	_left = _left - len;
	return 0;
}

// 先对现有容量进行判断，不够再扩展
int DataBuf::expandBuf(int need)
{
	moveHead();//先移到头

	if (_left >= need) { //此时如果还是空间不够就expand
		return 0;
	}

	int newLen = _len<<1;//增长2倍
	int realNeed = _len + need;//总共需要的长度

	while (newLen < realNeed) {
		newLen <<= 1;
	}

	char* newData = new char[newLen];

	if (newData == NULL) {
		return -1;
	}

	memcpy(newData, _data, _len);
	delete[] _data;
	// _head, _tail此时不需要改变
	_data = newData;
	_len = newLen;
	_left = _len - _tail;

	return 0;
}

