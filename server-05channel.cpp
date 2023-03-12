#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024


int main()
{

    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 9999);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Epoll *ep = new Epoll();
    serv_sock->setnonblocking();
    Channel *servChannel = new Channel(ep, serv_sock->getFd());
    servChannel->enableReading();
    while (true)
    {
        std::vector<Channel *> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        for (int i = 0; i < nfds; i++)
        {
            int chfd = activeChannels[i]->getFd();
            if (chfd == serv_sock->getFd())
            {                                               //新客户端连接
                InetAddress *clnt_addr = new InetAddress(); //会发生内存泄露！没有delete
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                Channel *clntChannel = new Channel(ep, clnt_sock->getFd());
                clntChannel->enableReading();
            }
            else if (activeChannels[i]->getRevents() & EPOLLIN)
            { //可读事件
                handleReadEvent(activeChannels[i]->getFd());
            }
            else
            { //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
}