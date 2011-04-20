#ifndef KV_PARSER_H_
#define KV_PARSER_H_

#include <map>
#include <string>

int parse_string (const char* str, std::map<std::string,std::string> &result);

class kv_parser
{
public:
    kv_parser(const char* str);
    ~kv_parser();

    void clear();
    const char* get(const char* key);
    void set(const char* key, const char* value);
    std::string make_str();

private:
    std::map<std::string, std::string> _kv;
};

#endif
