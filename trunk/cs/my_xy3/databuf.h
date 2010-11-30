#ifndef _DATABUF_H
#define _DATABUF_H

#include <iostream>
using namespace std;

typedef unsigned char byte;

// ����queue��ʽʵ�֣���β�ͼӣ���ͷ��ȡ
class DataBuf
{
private:
	char* _data;
	int _len;//�ܳ��ȣ�����
	int _head;//ͷָ��
	int _tail;//βָ��
	int _left;//ʣ����ó���
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
