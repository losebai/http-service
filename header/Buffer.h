#pragma once
#include <string>

class Buffer
{
private:
    std::string buf;

public:
    Buffer();
    ~Buffer();
    void append(const char *_str, int _size);

    ssize_t size();
    //返回指向以空结束的内容的const指针。 这是内部数据的句柄。 不要修改或可怕的事情可能发生
    const char *c_str();

    void clear();
    void getline();

    void setBuf(const char *);
};
