#pragma once
#include <map>
#include <vector>
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *loop;
    EventLoop *mainReactor; //只负责接受连接，然后分发给一个subReactor
    Acceptor *acceptor;
    std::map<int, Connection *> connections; // TCP连接
    std::vector<EventLoop *> subReactors;    //负责处理事件循环
    ThreadPool *thpool;                      //线程池

public:
    Server(EventLoop *);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *serv_sock);
    //删除该连接的回调函数
    void deleteConnetion(int sock);
};