#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop;
    Socket *sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    //简历连接
    void acceptConnection();
    //新建立连接的回调函数
    std::function<void(Socket*)> newConnectionCallback;
    //设置回调函数
    void setNewConnectionCallback(std::function<void(Socket*)>);
};

