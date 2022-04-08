#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main1() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET表示使用IPv4,SOCK_STREAM表示流格式,0表示根据前面的两个参数自动推导协议类型。

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //zero初始化这个结构体
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //设置ip, prot
    serv_addr.sin_port = htons(8888);

    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); // socket地址与文件描述符绑定：

    listen(sockfd, SOMAXCONN); // 监听这个socket端口

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);

    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    return 0;
}