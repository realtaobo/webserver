/*
 * @Autor: taobo
 * @Date: 2020-05-28 23:30:01
 * @LastEditTime: 2020-05-31 11:19:48
 * @Description: reactor，one loop per thread的事件源定义
 */ 
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include "EventLoop.h"


using namespace std;

class HttpData; //HTTP data package

class Channel
{
private:
    EventLoop* loop_;
    int fd_;
    __uint32_t events_  = 0;
    __uint32_t revents_ = 0;
    __uint32_t lastEvents_ = 0;
    //观测拥有本事件源的HTTP链接
    weak_ptr<HttpData> holder_;
private:
    /*HTTP头处理函数    */
    typedef function<void()> CallBack;
    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;
public:
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder);
    shared_ptr<HttpData> getHolder();
    void setReadHandler(CallBack &&readHandler) { //如名
        readHandler_ = readHandler; 
    }
    void setWriteHandler(CallBack &&writeHandler) {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler) {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler) { 
        connHandler_ = connHandler; 
    }
    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();
    void handleEvents();
    void setRevents(__uint32_t ev);

    void setEvents(__uint32_t ev) ;
    
    __uint32_t& getEvents() ;

    bool EqualAndUpdateLastEvents();

    __uint32_t getLastEvents();
};
