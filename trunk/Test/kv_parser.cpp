#include "kv_parser.h"

using namespace std;

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

KVParser::KVParser(const char* str)
{
    _kv.clear();

    parse_string(str, _kv);
}

KVParser::~KVParser()
{

}

void KVParser::clear()
{
    _kv.clear();
}

const char* KVParser::get(const char* key)
{
    string k(key);
    if (_kv.find(k) == _kv.end() ) {
        return NULL;
    } else {
        return _kv[k].c_str();
    }
}

void KVParser::set(const char* key, const char* value)
{
    string k(key);
    string v(value);

    _kv[k] = v;
}

std::string KVParser::make_str()
{
    string out;

    for (map<string, string>::const_iterator it = _kv.begin(); it != _kv.end(); ++it) {
        out += it->first;
        out += string("=");
        out += it->second;
        out += string(";");
    } 

    int len = out.size();
    if (len > 0) {
        out[len-1] = '\0';
    }

    return out;    
}
