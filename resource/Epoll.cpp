#include "headers/Epoll.h"
#include "headers/util.h"
#include "headers/Channel.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr){
    epfd = epoll_create1(0); // 用于创建一个 epoll 实例并返回对应的文件描述符。
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS); // bzero(events, sizeof(*events) * MAX_EVENTS) 的作用是将 events 数组的所有元素清零，以便下一次使用。
}

Epoll::~Epoll(){
    if(epfd != -1){
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

std::vector<Channel*> Epoll::poll(int timeout){
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++i){
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setReady(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoll::updateChannel(Channel *channel){
    int fd = channel->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents(); 
    // 更新或者新增epll事件
    if(!channel->getInEpoll()){
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll();
    } else{
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}

void Epoll::deleteChannel(Channel *channel){
    int fd = channel->getFd();
    errif(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll delete error");
    channel->setInEpoll(false);
}
