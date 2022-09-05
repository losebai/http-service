#include <iostream>
#include <unistd.h>
#include <string.h>
#include "headers/util.h"
#include "headers/Buffer.h"
#include "headers/InetAddress.h"
#include "headers/Socket.h"

using namespace std;

int main() {
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("150.95.9.112", 80);
    sock->connect(addr);

    int sockfd = sock->getFd();
    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();
    
    sendBuffer->setBuf("GET / HTTP/1.1 \nHost: hackr.jp \r\nConnection: keep-alive\r\nAccept: *\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: zh-CN\r\nUser-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.60 Safari/537.36 Edg/100.0.1185.29\r\nReferer: http://hackr.jp/");
    while(true){
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];    //这个buf大小无所谓
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("message from server: %s\n", readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
    delete addr;
    delete sock;
    return 0;
}