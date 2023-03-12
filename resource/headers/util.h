#pragma once
#include <arpa/inet.h>
#include <cerrno>

class Socket;

//判断错误
void errif(bool, const char *);
// 创建socket
sockaddr_in create_sockaddr(bool init);
//由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
void handleReadEvent(int);
// ET需要搭配非阻塞式socket使用
void setnonblocking(int fd);