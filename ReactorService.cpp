// #include "headers/Reactor.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

#include "Reactor.cpp"

using namespace std;


int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        cout << "socket error" << endl;
        exit(1);
    }

    int optval = 1;
    //设置 listenfd 套接字选项，使用 setsockopt 函数进行设置，
     if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        printf("setsockopt");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = inet_addr("172.26.59.146");
    if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(listenfd, SOMAXCONN) == -1) {
        perror("listen");
        return 1;
    }

    EventLoop* loop = new EventLoop();
    loop->eventLopp(listenfd);
}
