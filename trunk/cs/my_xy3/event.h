#ifndef EVENT_H
#define EVENT_H

#include "databuf.h"

#define NET_NEW 0
#define NET_DATA 1
#define NET_LEAVE 2
#define NET_TIMER 3

struct Event {
	int _event;
	int _hid;
	char _data[100];
	int _len;//_data的有效长度

	Event() {
		_event = -1;
		_hid = -1;
		_len = 0;
	}

	int addData(char* data, int len) {
		if (len > 100) {
			return -1;
		}

		memcpy(_data, data, len);
		_len = len;
		return 0;
	}

	void printData() {
		if (_event == NET_DATA) {
			cout<<"_event = "<<_event<<endl;
			cout<<"_hid = "<<_hid<<endl;
			cout<<"_len = "<<_len<<endl;
			cout<<"_data = "<<_data<<endl;
		}
	}
};

#endif
