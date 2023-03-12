#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    struct sockaddr_in serv_addr = create_sockaddr(true);
    errif(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    //创建一个epoll文件描述符并返回，失败则返回-1。
    int epfd = epoll_create1(0); //参数表示监听事件的大小，如超过内核会自动调整，已经被舍弃，无实际意义，传入一个大于0的数即可
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events)); // 初始化epoll_event数组
    bzero(&ev, sizeof(ev));

    ev.data.fd = sockfd;                         //该IO口为服务器socket fd
    ev.events = EPOLLIN | EPOLLET;               //  //在代码中使用了ET模式，且未处理错误，在day12进行了修复，实际上接受连接最好不要用ET模式
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); //将服务器socket fd添加到epoll

    while (true)
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); //获取有事件发生的fd
        errif(nfds == -1, "epoll wait error");
        for (int i = 0; i < nfds; ++i)
        { //处理这nfds个事件
            if (events[i].data.fd == sockfd)
            {                                 //发生事件的fd是服务器socket fd，表示有新客户端连接
                struct sockaddr_in clnt_addr; // 客户端
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len); // 等待连接。
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;                    //对于客户端连接，使用ET模式，可以让epoll更加高效，支持更多并发
                setnonblocking(clnt_sockfd);                      // // setnotblocking 将该文件描述符置为非阻塞状态
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev); //将该客户端的socket fd添加到epoll
            }
            else if (events[i].events & EPOLLIN)
            { //发生事件的是客户端，并且是可读事件（EPOLLIN）
                char buf[READ_BUFFER];
                while (true)
                { //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if (bytes_read > 0)
                    {
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
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
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd); //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            }
            else
            { //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}