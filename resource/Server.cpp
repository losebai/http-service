#include "headers/Server.h"
#include "headers/Socket.h"
#include "headers/Acceptor.h"
#include <functional>
#include "headers/ThreadPool.h"
#include "headers/EventLoop.h"
#include <string.h>
#include <unistd.h>
#include "headers/Connection.h"

#define READ_BUFFER 1024

// 1. 服务器一般只有一个main Reactor，有很多个sub Reactor。
//  2. 服务器管理一个线程池，每一个sub Reactor由一个线程来负责`Connection`上的事件循环，事件执行也在这个线程中完成。
//  3. main Reactor只负责`Acceptor`建立新连接，然后将这个连接分配给一个sub Reactor。
Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr)
{

    acceptor = new Acceptor(mainReactor); // Acceptor由且只由mainReactor负责
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency(); //线程数量，也是subReactor数量
    thpool = new ThreadPool(size);                  //新建线程池
    for (int i = 0; i < size; ++i)
    {
        subReactors.push_back(new EventLoop()); //每一个线程是一个EventLoop
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop); //开启所有线程的事件循环
    }
}

Server::~Server()
{
    delete acceptor;
    delete thpool;
}

void Server::handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    { //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        }
        else if (bytes_read == -1 && errno == EINTR)
        { //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { //非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd); //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

// 建立新的连接
void Server::newConnection(Socket *sock)
{
    if (sock->getFd() != -1)
    {
        int random = sock->getFd() % subReactors.size();
        Connection *conn = new Connection(subReactors[random], sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnetion, this, std::placeholders::_1); // 删除连接的回调。关闭的
        conn->setDeleteConnectionCallback(cb);
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnetion(int sockfd)
{
    if (sockfd != -1)
    {
        auto it = connections.find(sockfd);
        if (it != connections.end())
        {
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            // close(sockfd);       //正常
            delete conn; //会Segmant fault
        }
    }
}
