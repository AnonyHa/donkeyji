/*
��С����־�� ����cout&lt;&lt;"A"&lt;&lt;B&lt;&lt;"\n"һ�� �ղ�
��С����־��  ����cout<<"A"<<B<<"\n"һ��  ֧��  cout<<"A"<<B<<endl;

�����Ҫ �����Լ�����<<
*/

//H
#ifndef _MYLOG_HEADER_
#define _MYLOG_HEADER_

//#include "VString.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include<Windows.h>
#include <time.h>
#endif
using namespace std;
/*
���ܣ�д��־����
���ߣ�wangzhongwei
EMAIL��wzw200@sina.com

���� �÷���
MyLog Logout("Server.log");
int a=1;
char b[]="bb";
double d=0.5;

Logout<<a<<b<<d<<endl;
���д�ӡһ�к������endl���� "\n"��β
�������
*/
class MyLog
{
private:
	bool _IsTime;
	ofstream _Logout;
	char _Time[50];
	char _Path[1024];
private:
	char* GetTime();
public:
	MyLog(char *LogFilepath);
	~MyLog() {};
	MyLog& operator<<(char* s);
	MyLog& operator<<(int s);
	MyLog& operator<<(double s);
	MyLog& operator<<(string s);
	MyLog& operator<<(basic_ostream<char, char_traits<char> >& (*_Pfn)(basic_ostream<char, char_traits<char> >&)) {
		// write endl to log using _Pfn  ����endl
		_Logout.open(_Path,ios::app);

		if(_IsTime) {
			_IsTime=false;
			_Logout<<GetTime()<<":";
		}

		_IsTime=true;
		_Logout<<"\n";
		_Logout.close();
		return (*this);
	}
};
#endif

MyLog::MyLog(char *LogFilepath)
{
	memset(_Path,'\0',1024);
	strcat(_Path,LogFilepath);
	_IsTime=true;
}

char* MyLog::GetTime()
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(_Time,"%d-%d-%d %d:%d:%d",st.wYear, st.wMonth ,st.wDay,st.wHour ,st.wMinute,st.wSecond);
	return _Time;
#else
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep);   //get server's time

	if(p==NULL) {
		return NULL;
	}

	//create specific datetime format
	sprintf(_Time,"%d-%d-%d %d:%d:%d",p->tm_year+1900, p->tm_mon+1 ,p->tm_mday ,p->tm_hour ,p->tm_min,p->tm_sec);
	return _Time;
#endif
	return 0;
}

MyLog& MyLog::operator<<(char* s)
{
	_Logout.open(_Path,ios::app);

	if(_IsTime) {
		_IsTime=false;
		_Logout<<GetTime()<<":";
	}

	if(strcmp(s,"\n")==0) {
		_IsTime=true;
	}

	_Logout<<s;
	_Logout.close();

	return (*this);
}

MyLog& MyLog::operator<<(int  s)
{
	_Logout.open(_Path,ios::app);

	if(_IsTime) {
		_IsTime=false;
		_Logout<<GetTime()<<":";
	}

	_Logout<<s;
	_Logout.close();

	return (*this);
}

MyLog& MyLog::operator<<(double s)
{
	_Logout.open(_Path,ios::app);

	if(_IsTime) {
		_IsTime=false;
		_Logout<<GetTime()<<":";
	}

	_Logout<<s;
	_Logout.close();

	return (*this);
}

MyLog& MyLog::operator<<(string s)
{
	_Logout.open(_Path,ios::app);

	if(_IsTime) {
		_IsTime=false;
		_Logout<<GetTime()<<":";
	}

	_Logout<<(char *)s.c_str();
	_Logout.close();

	return (*this);
}

// -----------
// test
// -----------
int main()
{
	MyLog Logout("1.log");

	Logout<<"A"<<"B"<<"\n";
	Logout<<"CC"<<123456<<"\n";
	Logout<<endl;
	Logout<<"d"<<"\n";

	double a=0.5;
	int b=5;
	char aa[10]="aabb";
	string vs="ddee";
	Logout<<"double="<<a<<" int="<<b<<" char aa="<<aa<<" VString="<<vs<<endl<<"eeffggg"<<endl;
	int i=0;

	return 1;
}
