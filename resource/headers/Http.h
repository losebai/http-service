#pragma once
#include <string.h>
#include <string>

struct Header
{
    /* data */
    std::string uri;
    std::string menthd;
    std::string version;
    std::string host;
    std::string authority;
    std::string path;
    std::string menthd;
    std::string Content_type;
    std::string Content_lenth;
    std::string connection;
    std::string accept_encoding;
    std::string date;
    std::string referer;
    std::string cookie;
};

class Http
{
private:
    struct Header header;
    std::string body;

public:
    Http();
    ~Http();
}