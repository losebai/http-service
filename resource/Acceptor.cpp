#include "headers/Acceptor.h"
#include "headers/Socket.h"
#include "headers/InetAddress.h"
#include "headers/Channel.h"


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

