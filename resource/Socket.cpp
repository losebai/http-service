#include "headers/Socket.h"
#include "headers/InetAddress.h"
#include "headers/util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <cstring>
#include <string>
Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0); // ipv 和 流式套接字
    errif(fd == -1, "socket create error");
}
Socket::Socket(int _fd) : fd(_fd)
{
    errif(fd == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        printf("fd close: %d", fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *_addr)
{
    struct sockaddr_in addr = _addr->getAddr();
    errif(::bind(fd, (sockaddr *)&addr, sizeof(addr)) == -1, "socket bind error");
}

void Socket::listen()
{  //，用于将一个套接字 fd 标记为被动套接字，表示该套接字用于监听来自其它套接字的连接请求。
    //SOMAXCONN 参数指定了内核允许在套接字未完成 3 次握手之前排队的最大连接数。
    //因此，调用 listen 函数后，套接字 fd 会进入监听状态，可以开始接收客户端连接请求
   // 。如果请求被接受，将返回一个新的已连接套接字（connected socket），它用于与客户端进行通信。
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}
void Socket::setnonblocking()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *_addr)
{
    struct sockaddr_in addr;
    int clnt_sockfd = -1;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    if (fcntl(fd, F_GETFL) & O_NONBLOCK) // 检查文件描述符 fd 是否设置了 O_NONBLOCK 非阻塞标志位
    {
        while (true)
        {
            clnt_sockfd = ::accept(fd, (sockaddr *)&addr, &addr_len);
            if (clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            {
                // printf("no connection yet\n");
                continue;
            }
            else if (clnt_sockfd == -1)
            {
                errif(true, "socket accept error");
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        clnt_sockfd = ::accept(fd, (sockaddr *)&addr, &addr_len);
        errif(clnt_sockfd == -1, "socket accept error");
    }
    _addr->setInetAddr(addr);
    return clnt_sockfd;
}

void Socket::connect(InetAddress *_addr)
{
    // for client socket
    struct sockaddr_in addr = _addr->getAddr();
    if (fcntl(fd, F_GETFL) & O_NONBLOCK)
    {
        while (true)
        {
            int ret = ::connect(fd, (sockaddr *)&addr, sizeof(addr));
            if (ret == 0)
            {
                break;
            }
            else if (ret == -1 && (errno == EINPROGRESS))
            {
                continue;
                /* 连接非阻塞式sockfd建议的做法：
                    The socket is nonblocking and the connection cannot be
                  completed immediately.  (UNIX domain sockets failed with
                  EAGAIN instead.)  It is possible to select(2) or poll(2)
                  for completion by selecting the socket for writing.  After
                  select(2) indicates writability, use getsockopt(2) to read
                  the SO_ERROR option at level SOL_SOCKET to determine
                  whether connect() completed successfully (SO_ERROR is
                  zero) or unsuccessfully (SO_ERROR is one of the usual
                  error codes listed here, explaining the reason for the
                  failure).
                  这里为了简单、不断连接直到连接完成，相当于阻塞式
                */
            }
            else if (ret == -1)
            {
                errif(true, "socket connect error");
            }
        }
    }
    else
    {
        errif(::connect(fd, (sockaddr *)&addr, sizeof(addr)) == -1, "socket connect error");
    }
}

int Socket::getFd()
{
    return fd;
}
