#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"


int main()
{
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 9999);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Epoll *ep = new Epoll();
    serv_sock->setnonblocking();
    ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    while (true)
    {
        std::vector<epoll_event> events = ep->poll(); // 获取所有fd
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == serv_sock->getFd())
            {                                                                 //新客户端连接
                InetAddress *clnt_addr = new InetAddress();                   //会发生内存泄露！没有delete
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr)); //会发生内存泄露！没有delete
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            }
            else if (events[i].events & EPOLLIN)
            { //可读事件
                handleReadEvent(events[i].data.fd);
            }
            else
            { //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}