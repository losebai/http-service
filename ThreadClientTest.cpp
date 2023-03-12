#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include "headers/util.h"
#include "headers/Buffer.h"
#include "headers/InetAddress.h"
#include "headers/Socket.h"
#include "headers/ThreadPool.h"

using namespace std;

void oneClient(int msgs, int wait){
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("172.26.59.146", 9999);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    sleep(wait);
    int count = 0;
    while(count < msgs){
        sendBuffer->setBuf("I'm client!");
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
                printf("count: %d, message from server: %s\n", count++, readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
        printf("fd wait..: %d\n", sockfd);
    }
    close(sockfd);
    printf("fd close:%d\n", sockfd);
    delete addr;
    delete sock;
}

int main(int argc, char *argv[]) {
    int threads = 100;
    int msgs = 100;
    int wait = 0;


    ThreadPool *poll = new ThreadPool(threads);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++i){
        poll->add(func);
    }
    delete poll;
    return 0;
}
