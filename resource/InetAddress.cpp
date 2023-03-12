#include "headers/InetAddress.h"
#include <cstring>
InetAddress::InetAddress()
{
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char *ip, uint16_t port)
{
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress()
= default;
void InetAddress::setInetAddr(sockaddr_in _addr)
{
    addr = _addr;
}
sockaddr_in InetAddress::getAddr()
{
    return addr;
}
