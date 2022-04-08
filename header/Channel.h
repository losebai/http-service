#pragma once
#include <functional>
class EventLoop;
class Epoll;
class Channel
{

private:
    EventLoop *loop;
    // Epoll *ep; //Epoll
    int fd;
    uint32_t events;
    // revents`表示在`epoll`返回该`Channel`时文件描述符正在发生的事件
    uint32_t revents;
    // inEpoll`表示当前`Channel`是否已经在`epoll`红黑树中
    uint32_t ready;
    bool inEpoll;
    bool useThreadPool;
    std::function<void()> callback;
    std::function<void()> readCallback;
    std::function<void()> writeCallback;

public:
    Channel(Epoll *_ep, int _fd);       // Epoll
    Channel(EventLoop *_loop, int _fd); // EventLoop
    ~Channel();

    //如`Channel`不在epoll红黑树中，则添加，否则直接更新`Channel`
    void handleEvent();
    //事件读取
    void enableRead();
    uint32_t getReady();
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();

    bool getInEpoll();
    void setInEpoll(bool _in = true);

    // 设置事件
    void setRevents(uint32_t);
    //回调函数
    void setCallback(std::function<void()>);

    //使用et
    void useET();
    //读取
    void setReady(uint32_t);
    //读取回调
    void setReadCallback(std::function<void()>);
    //读取线程池
    void setUseThreadPool(bool use = true);
};