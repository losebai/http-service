#pragma once
#include <arpa/inet.h>

class InetAddress
{
private:
    struct sockaddr_in addr;

public:
    socklen_t addr_len;
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();

    sockaddr_in getAddr();
    void setInetAddr(sockaddr_in _addr);
};
