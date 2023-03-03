#include <iostream>
#include <unistd.h>
#include <string.h>
#include "headers/util.h"
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

    sendBuffer->setBuf("GET / HTTP/1.1 \r\nHost: hackr.jp \r\nConnection: keep-alive\r\nAccept: *\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: zh-CN\r\nUser-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.60 Safari/537.36 Edg/100.0.1185.29\r\nReferer: http://hackr.jp/");
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
    write(sockfd, sendBuffer->c_str(), sendBuffer->size());
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
    delete sock; // 会产生一个读取事件
    printf("close : %d \r\n", sockfd);
    return 0;
}