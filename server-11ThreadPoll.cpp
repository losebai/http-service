
#include "header/Server.h"
#include "header/EventLoop.h"

int main(){
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop();
    return 0;
}