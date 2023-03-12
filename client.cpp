#include <iostream>
#include <unistd.h>
#include <string.h>
#include "headers/Buffer.h"
#include "headers/InetAddress.h"
#include "headers/Socket.h"

using namespace std;

int main()
{
    Socket *sock = new Socket();
    printf("create fd : %d ", sock->getFd());
    InetAddress *addr = new InetAddress("172.26.59.146", 9999);
    sock->connect(addr);

    int sockfd = sock->getFd();
    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    sendBuffer->setBuf(" write one \n");
    int num = 0;
    std::cin >> num;
    ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
    printf("本次写入大小为：%ld\r\n", write_bytes);
    if (write_bytes == -1)
    {
        printf("socket already disconnected, can't write any more!\n");
    }
    int already_read = 0;
    char buf[1024]; // 这个buf大小无所谓
    bzero(&buf, sizeof(buf));
    sendBuffer->setBuf(" write two \n");
    write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
    printf("本次写入大小为：%ld\r\n", write_bytes);
    // ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    // if (read_bytes > 0)
    // {
    //     readBuffer->append(buf, read_bytes);
    //     already_read += read_bytes;
    // }
    // printf("message from server: %s\n", readBuffer->c_str());
    // readBuffer->clear();
    delete addr;
    std::cin >> num;
    // 当客户端关闭连接时，服务端会接收到一个FIN（结束）的数据包，
    // 因此服务端会触发该客户端对应的socket文件描述符上
    // 的可读事件,从而调用相应的事件处理函数。在事件处理函数中，可以检测到该客户端连接已经关闭，然后进行相关的资源释放操作。
    delete sock;
    printf("close : %d \r\n", sockfd);
    return 0;
}