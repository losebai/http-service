cmake_minimum_required(VERSION 3.16.3)
project(http_server)

set(CMAKE_CXX_STANDARD 11)

include_directories(resource/headers)
add_compile_options(-pthread)
include_directories(resource)

add_executable(http_server
#        resource/headers/Acceptor.h
        resource/headers/Buffer.h
#        resource/headers/Channel.h
#        resource/headers/Connection.h
#        resource/headers/Epoll.h
#        resource/headers/EventLoop.h
#        resource/headers/Http.h
        resource/headers/InetAddress.h
#        resource/headers/main.cpp
        resource/headers/Reactor.h
#        resource/headers/Server.h
        resource/headers/Socket.h
#        resource/headers/ThreadPool.h
        resource/headers/util.h
#        resource/Acceptor.cpp
        resource/Buffer.cpp
#        resource/Channel.cpp
#        resource/Connection.cpp
#        resource/Epoll.cpp
#        resource/EventLoop.cpp
        resource/InetAddress.cpp
        resource/Reactor.cpp
#        resource/Server.cpp
        resource/Socket.cpp
#        resource/ThreadPool.cpp
        resource/util.cpp
#        client.cpp
#        main.cpp
#        Nioclient.cpp
        ReactorService.cpp
#        server-01.cpp
#        server-03epoll.cpp
#        server-04epoll.cpp
#        server-05channel.cpp
#        server-06EventLoop.cpp
#        server-07Acceptor.cpp
#        server-08Connetion.cpp
#        server-2.cpp
#        ThreadClientTest.cpp
#        ThreadPollTest.cpp resource/headers/InetAddress.h resource/headers/InetAddress.h
        )
