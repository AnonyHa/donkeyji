#include <map>
#include <string>
#include <iostream>

int 
parse_string(const char* str, std::map<std::string,std::string>& result)
{
	int st = 0;
	const char* key_start;
	const char* key_end;
	const char* value_start;
	const char* value_end;

	const char* p = str;

	size_t len = strlen(str);
	if (len <= 0) {
		std::cout<<"str is null"<<std::endl;
		return st;
	}

	std::string key;
	std::string value;

	while (*p != '\0') {
		while (*p != '\0' && *p == ' ') {
			p++;
		}
		key_start = p;
		while (*p != '\0' && *p != ' ' && *p != '=') {
			p++;
		}
		key_end = p;
		if (key_end == key_start) {//syntax error
			std::cout<<"111"<<std::endl;
			st = -1;
			break;
		}

		std::string key(key_start, 0, key_end-key_start);

		while (*p != '\0' && *p == ' ') {
			p++;
		}
		if (*p != '=') {//must be '='
			std::cout<<"222"<<std::endl;
			st = -1;
			break;
		}
		p++;
		while (*p != '\0' && *p == ' ') {
			p++;
		}
		value_start = p;

		while (*p != '\0' && *p != ' ' && *p != ';') {
			p++;
		}
		value_end = p;
		if (value_start == value_end) {
			st = -1;
			std::cout<<"333"<<std::endl;
			break;
		}

		std::string value(value_start, 0, value_end-value_start);

		result[key] = value;

		while (*p != '\0' && *p == ' ') {
			p++;
		}
		if (*p != ';') {
			std::cout<<"444"<<std::endl;
			st = -1;
			break;
		}
		p++;//skip to the char behind ';'
	}
	return st;
}

int main()
{
	std::map<std::string, std::string> ret;
	
	//const char* src = " a =1; bc= 2; mn=  3;  xy = bcd;";
	const char* src = "";
	int st = parse_string(src, ret);
	if (st == 0) {
		std::map<std::string, std::string>::iterator iter;
		for (iter=ret.begin(); iter!=ret.end(); iter++) {
			std::cout<<"key: "<<iter->first<<std::endl;
			std::cout<<"value: "<<iter->second<<std::endl;
		}
	} else {
		std::cout<<"syntax error"<<std::endl;
	}
	return 0;
}
