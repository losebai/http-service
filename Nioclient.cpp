#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

int main()
{
    const char *HOST = "172.26.59.146";
    int PORT = 9999;

    // 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // 设置非阻塞模式
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // 连接服务器
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(HOST);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 && errno != EINPROGRESS)
    {
        std::cerr << "Failed to connect server." << std::endl;
        return -1;
    }

    // 创建 epoll 实例并添加 sockfd 事件
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        std::cerr << "Failed to create epoll instance." << std::endl;
        return -1;
    }
    struct epoll_event ev, events[MAX_EVENTS];
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
    ev.data.fd = sockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        std::cerr << "Failed to add sockfd to epoll." << std::endl;
        return -1;
    }

    // 监听 epoll 实例中的事件
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
        std::cerr << "Failed to wait events." << std::endl;
        return -1;
    }

    // 处理 sockfd 事件
    for (int i = 0; i < nfds; i++)
    {
        if (events[i].data.fd == sockfd && (events[i].events & EPOLLOUT))
        {
            // 连接成功
            std::cout << "Connected to server." << std::endl;

            // 从 epoll 中删除 sockfd 事件
            if (epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL) == -1)
            {
                std::cerr << "Failed to delete sockfd from epoll." << std::endl;
                return -1;
            }

            // 发送数据
            const char *data = "Hello, world!";
            size_t len = strlen(data);
            ssize_t bytes_sent = send(sockfd, data, len, MSG_NOSIGNAL);
            if (bytes_sent == -1)
            {
                std::cerr << "Failed to send data." << std::endl;
                return -1;
            }
            std::cout << "Sent " << bytes_sent << " bytes of data." << std::endl;

            // 关闭 socket
            close(sockfd);
            break;
        }
    }
}
