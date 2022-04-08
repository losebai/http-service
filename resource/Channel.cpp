#include "headers/Channel.h"
#include "headers/EventLoop.h"
#include <unistd.h>
#include "headers/Socket.h"
#include <sys/epoll.h>

// Channel::Channel(EventLoop *_loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), inEpoll(false)
// {
// }

// day11
Channel::Channel(EventLoop *_loop, int _fd)
    : loop(_loop), fd(_fd), events(0), ready(0), inEpoll(false), useThreadPool(true) {}

Channel::~Channel()
{
    // delete ep;
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

// void Channel::handleEvent()
// {
//     callback();
// }

// day11
void Channel::handleEvent()
{
    // if (ready & (EPOLLIN | EPOLLPRI))
    // {
    //     if (useThreadPool)
    //         loop->addThread(readCallback);
    //     else
    //         readCallback();
    // }
    // if (ready & (EPOLLOUT))
    // {
    //     if (useThreadPool)
    //         loop->addThread(writeCallback);
    //     else
    //         writeCallback();
    // }
    if (ready & (EPOLLIN | EPOLLPRI))
    {
        readCallback();
    }
    if (ready & (EPOLLOUT))
    {
        writeCallback();
    }
}

void Channel::enableRead()
{
    events |= EPOLLIN | EPOLLPRI;
    loop->updateChannel(this);
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}
uint32_t Channel::getRevents()
{
    return revents;
}
uint32_t Channel::getReady()
{
    return ready;
}
bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll(bool _in)
{
    inEpoll = _in;
}

// void Channel::setEvents(uint32_t _ev){
//     events = _ev;
// }

void Channel::setRevents(uint32_t _ev)
{
    revents = _ev;
}

void Channel::setCallback(std::function<void()> _cb)
{
    callback = _cb;
}

void Channel::useET()
{
    events |= EPOLLET;
    loop->updateChannel(this);
}

void Channel::setReady(uint32_t _ev)
{
    ready = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb)
{
    readCallback = _cb;
}
void Channel::setUseThreadPool(bool use)
{
    useThreadPool = use;
}