#include "kv_parser.h"

int parse_string (const char* str, std::map<std::string,std::string> &result)
{
    int key_start = 0;
    int key_end = 0;
    int value_start = 0;
    int value_end = 0;

	int has_kv = 0;

    while (str[key_start] != '\0') {
        // key
        while (str[key_start] != '\0' && (str[key_start] == ';' || str[key_start] == ' ') ) {
            ++key_start;
        }

        key_end = key_start;
        while (str[key_end] != '\0' && str[key_end] != ' ' && str[key_end] != '=') {
            ++key_end;
        }

        // value
        value_start = key_end;
        while (str[value_start] != '\0' && (str[value_start] == '=' || str[value_start] == ' ') ) {
            ++value_start;
        }
        value_end = value_start;
        while (str[value_end] != '\0' && str[value_end] != ' ' && str[value_end] != ';') {
            ++value_end;
        }

        if ((key_end > key_start) && (value_end > value_start) ) {
            std::string key = std::string(str, key_start, key_end-key_start);
            std::string value = std::string(str, value_start, value_end-value_start);

            result[key] = value;

			has_kv = 1;
        }

        key_start = value_end;
    }

    return has_kv;
}

kv_parser::kv_parser(const char* str)
{
    _kv.clear();

    parse_string(str, _kv);
}

kv_parser::~kv_parser()
{

}

void kv_parser::clear()
{
    _kv.clear();
}

const char* kv_parser::get(const char* key)
{
	std::string k(key);
    if (_kv.find(k) == _kv.end() ) {
        return NULL;
    } else {
        return _kv[k].c_str();
    }
}

void kv_parser::set(const char* key, const char* value)
{
	std::string k(key);
	std::string v(value);

    _kv[k] = v;
}

std::string kv_parser::make_str()
{
	std::string out;

    for (std::map<std::string, std::string>::const_iterator it = _kv.begin(); it != _kv.end(); ++it) {
        out += it->first;
        out += std::string("=");
        out += it->second;
        out += std::string(";");
    } 

    int len = out.size();
    if (len > 0) {
        out[len-1] = '\0';
    }

    return out;    
}
