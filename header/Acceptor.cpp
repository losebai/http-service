#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

//
// Acceptor::Acceptor(EventLoop *_loop) :loop(_loop){
//     sock = new Socket();
//     addr = new InetAddress("127.0.0.1", 9999);
//     sock->bind(addr);
//     sock->listen();
//     sock->setnonblocking();
//     acceptChannel = new Channel(loop, sock->getFd());
//     std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
//     acceptChannel->setCallback(cb);
//     acceptChannel->enableReading();
// }

// day11
//  今天还发现了之前版本的一个缺点：对于`Acceptor`，接受连接的处理时间较短、报文数据极小，并且一般不会有特别多的新连接在同一时间到达，所以`Acceptor`没有必要采用epoll ET模式，也没有必要用线程池。由于不会成为性能瓶颈，为了简单最好使用阻塞式socket，故今天的源代码中做了以下改变：
//  1. Acceptor socket fd（服务器监听socket）使用阻塞式
//  2. Acceptor使用LT模式，建立好连接后处理事件fd读写用ET模式
//  3. Acceptor建立连接不使用线程池，建立好连接后处理事件用线程池
Acceptor::Acceptor(EventLoop *_loop) : loop(_loop), sock(nullptr), acceptChannel(nullptr)
{
    sock = new Socket();
    InetAddress *addr = new InetAddress("172.26.59.146", 9999);
    sock->bind(addr);
    sock->listen();
    acceptChannel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setReadCallback(cb);
    acceptChannel->enableRead();
    acceptChannel->setUseThreadPool(false);
    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock;
    delete acceptChannel;
}

// void Acceptor::acceptConnection(){
//     newConnectionCallback(sock);
// }

// day11
void Acceptor::acceptConnection()
{
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr)); // 等待连接
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setnonblocking();
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> _cb)
{
    newConnectionCallback = _cb;
}

