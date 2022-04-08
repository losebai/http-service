#pragma once

class InetAddress;
class Socket
{
private:
    int fd;

public:
    Socket();
    Socket(int _fd);
    ~Socket();

    // 绑定地址
    void bind(InetAddress *);
    // 监听
    void listen();
    // setnotblocking 将该文件描述符置为非阻塞状态
    void setnonblocking();
    // 等待连接
    int accept(InetAddress *);
    // 获取fd对象
    int getFd();
    // 连接
    void connect(InetAddress *);
};
