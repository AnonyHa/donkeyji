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

class my_map
{
private:
	std::map<std::string, std::string> _tbl;
public:
	my_map(const char* str);
public:
	void clear();
	const char* get(const char* key);
	void set(const char* key, const char* value);
	void my_make_str();
public: 
	void print_tbl();
};

my_map::my_map(const char* str)
{
	int r = parse_string(str, _tbl);
	if (r != 0) {
		throw "syntax error";
	}
}

const char* my_map::get(const char* key)
{
	std::map<std::string, std::string>::iterator iter;
	iter = _tbl.find(key);
	if (iter == _tbl.end()) {
		std::cout<<"not found"<<std::endl;
		return NULL;
	} else {
		std::cout<<"found, value="<<iter->second.c_str()<<std::endl;
		return iter->second.c_str();
	}
}

void my_map::set(const char* key, const char* value)
{
	if (key == NULL) {
		return;
	}
	std::pair<std::map<std::string, std::string>::iterator, bool> ins_pair;
	//ins_pair = _tbl.insert(std::pair<std::string, std::string>(key, value));
	//if (ins_pair.second == true)
	_tbl[key] = value;
}


void my_map::print_tbl()
{
	std::map<std::string, std::string>::iterator iter;
	for (iter=_tbl.begin(); iter!=_tbl.end(); iter++) {
		std::cout<<"key: "<<iter->first<<"    "<<"value: "<<iter->second<<std::endl;
	}
}

void my_map::clear()
{
	/*
	std::map<std::string, std::string>::iterator iter;
	for (iter=_tbl.begin(); iter!=_tbl.end(); iter++) {
		_tbl.erase(iter);
	}
	*/
	_tbl.clear();
}


int main()
{
	//std::map<std::string, std::string> ret;
	
	const char* src = " a =1; bc= 2; mn=  3;  xy = bcd;";
	//const char* src = "";
	/*
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
	*/
	my_map mm(src);
	mm.print_tbl();
	std::cout<<"\n\n";
	mm.clear();
	//std::cout<<"after clear"<<std::endl;
	mm.set("huji", "donitz");
	mm.set("mn", "4");
	mm.print_tbl();
	//mm.get("huji");
	return 0;
}
