#include<iostream>
#include<string>
#include"ThreadPool.h"

void print(int a, double b, const char *c, std::string d){
    std::cout << a << b << c << d << std::endl;
}

void test(){
    std::cout << "test"<< std::endl;
}

int main(){
    ThreadPool *ThreadPool = new ThreadPool();
    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    ThreadPool->add(func);

    auto basicLambda = [] { std::cout << "Hello, world!" << std::endl; };
    ThreadPool->add(basicLambda);
    delete ThreadPool;
    return 0;

}