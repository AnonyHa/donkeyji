#ifndef _DATABUF_H
#define _DATABUF_H

#include <iostream>
using namespace std;

typedef unsigned char byte;

// 采用queue方式实现，从尾巴加，从头读取
class DataBuf
{
private:
	char* _data;
	int _len;//总长度，不变
	int _head;//头指针
	int _tail;//尾指针
	int _left;//剩余可用长度
public:
	DataBuf(int len = 1024);
	~DataBuf();
public:
	void moveHead();
	int cutData(int len);
	char* getPack(short& size);
	int getDataSize();
	char* getSendData();
	void printData();
	int appendRawData(byte* data, int len);
	int appendPack(byte* data, int len);
	int expandBuf(int need);
};

#endif
