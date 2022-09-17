#include "headers/Connection.h"
#include "headers/Socket.h"
#include "headers/Channel.h"
#include <unistd.h>
#include <string.h>
#include "headers/util.h"
#include "headers/Buffer.h"
#define READ_BUFFER 1024
#include <iostream>

// Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr){
//     channel = new Channel(loop, sock->getFd());
//     std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
//     channel->setCallback(cb); // 回调
//     channel->enableReading(); //监听
// }

// day 11
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), inBuffer(new std::string()), readBuffer(nullptr)
{
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();
    channel->useET(); // 使用et
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    channel->setUseThreadPool(true);
    readBuffer = new Buffer();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::echo(int sockfd)
{
    char buf[1024]; //这个buf大小无所谓
    while (true)
    { //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        { //客户端正常中断、继续读取
            printf("continue reading\n");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { //非阻塞IO，这个条件表示数据全部读取完毕
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            // errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            send(sockfd);
            readBuffer->clear();
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sockfd); //多线程会有bug
            break;
        }
        else
        {
            printf("Connection reset by peer\n");
            deleteConnectionCallback(sockfd); //会有bug，注释后单线程无bug
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb)
{
    deleteConnectionCallback = _cb;
}

void Connection::send(int sockfd)
{
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int data_size = readBuffer->size();
    int data_left = data_size;
    while (data_left > 0)
    {
        std::string sendStr = "HTTP/1.1 200 OK\n";
        sendStr += "Date: Mon, 27 Jul 2009 12:28:53 GMT\n";
        sendStr += "Server: Apache/2.2.14 (Win32)\n";
        sendStr += "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n";
        sendStr += "Content-Length: " + data_size;
        sendStr += "\n";
        sendStr += "Content-Type: text/html\n";
        sendStr += "Connection: Closed\n";
        sendStr += "\n";
        sendStr += "<html><body>";
        sendStr += "<h1> This is the HTTP packet you requested</h1>";
        bool is = false;
        std::string temp = "";
        for(int i = 0; i < data_size;i++){
            if(buf[i] == '\n'){
                sendStr += "<h2>";
                is = true;
            }
            if(is){
                 sendStr += "</h2>";
                 is = false;
               
                  temp = "";
            }
            sendStr += buf[i];
            temp +=  buf[i];
          
        }
        sendStr += "</body></html>";
        ssize_t bytes_write = write(sockfd, sendStr.c_str(), sendStr.length());
        if (bytes_write == -1 && errno == EAGAIN)
        {
            break;
        }
        data_left -= bytes_write;

    }
}