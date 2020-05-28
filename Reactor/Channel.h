/*
 * @Autor: taobo
 * @Date: 2020-05-28 23:30:01
 * @LastEditTime: 2020-05-29 00:13:14
 * @Description: reactor，one loop per thread的事件源定义
 */ 
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <memory>


using namespace std;

class EventLoop;
class HttpData; //HTTP data package

class Channel
{
private:
    EventLoop* loop_;
    int fd_;
    __uint32_t events_ ;
    __uint32_t revents_;
    __uint32_t lastEvents_;

    weak_ptr<HttpData> holder_;
private:
    /*HTTP头处理函数    */
    int parse_URI();
    int parse_Headers();
    int analysisRequest();

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

    void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }
    shared_ptr<HttpData> getHolder() {
        shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }
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
    //EPOLLET： 将 EPOLL设为边缘触发(Edge Triggered)模式（默认为水平触发）
    //EPOLLONESHOT： 只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

    //EPOLLIN       连接到达；有数据来临；
    //EPOLLOUT      有数据要写；             
    //EPOLLPRI      表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
    //EPOLLERR      表示对应的文件描述符发生错误；
    //EPOLLHUP      表示对应的文件描述符被挂断；
    /*EPOLLRDHUP    有些系统检测不到，可以使用EPOLLIN，read返回0，删除掉事件，关闭close(fd);
                    若可以检测到则表示对端已关闭*/ 
    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();
    void handleEvents() {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return;
        }
        if (revents_ & EPOLLERR) {
            if (errorHandler_) errorHandler_();
            events_ = 0;
            return;
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
        handleConn();
    }
    void setRevents(__uint32_t ev) { revents_ = ev; }

    void setEvents(__uint32_t ev) { events_ = ev; }
    __uint32_t &getEvents() { return events_; }

    bool EqualAndUpdateLastEvents() {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }

    __uint32_t getLastEvents() { return lastEvents_; }
};
