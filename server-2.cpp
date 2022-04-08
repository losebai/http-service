#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	errif(sockfd == -1, "socket create error");
    struct sockaddr_in serv_addr = create_sockaddr(true);
	errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
	errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

	struct sockaddr_in clnt_addr = create_sockaddr(false);
	socklen_t clnt_addr_len = sizeof(clnt_addr);

	int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
	errif(clnt_sockfd == -1, "socket accept error");

	printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    while (true) {
        char buf[1024]; // 缓存区 
        bzero(&buf, sizeof(buf)); //初始化缓存区
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
        if (read_bytes > 0) {
            printf("message from client fd %d: %s\n", clnt_sockfd, buf);
            write(clnt_sockfd, buf, sizeof(buf));  //将相同的数据写回到客户端
        }
        else if (read_bytes == 0) {
            printf("client fd %d disconnected\n", clnt_sockfd);
            close(clnt_sockfd);
            break;
        }
        else if (read_bytes == -1) {
            close(clnt_sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
   return 0;
}
