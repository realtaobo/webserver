/*
 * @Autor: taobo
 * @Date: 2020-05-29 09:03:26
 * @LastEditTime: 2020-05-29 09:28:25
 */ 
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <memory>

#include "Channel.h"

using namespace std;

Channel::Channel(EventLoop *loop):loop_(loop){}

Channel::Channel(EventLoop *loop, int fd):loop_(loop),fd_(fd){}

Channel::~Channel(){}

void Channel::setHolder(std::shared_ptr<HttpData> holder) 
{ 
    holder_ = holder; 
}

shared_ptr<HttpData> Channel::getHolder() {
    shared_ptr<HttpData> ret(holder_.lock());
    return ret;
}

int Channel::getFd()
{
    return fd_;
}

void Channel::setFd(int fd)
{
    fd_ = fd;
}

void Channel::handleRead() 
{
  if (readHandler_) {
    readHandler_();
  }
}

void Channel::handleWrite() 
{
  if (writeHandler_) {
    writeHandler_();
  }
}

void Channel::handleConn() 
{
  if (connHandler_) {
    connHandler_();
  }
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
void Channel::handleEvents() 
{
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
void Channel::handleError(int fd, int err_num, std::string short_msg){}

void Channel::setRevents(__uint32_t ev) { revents_ = ev; }

void Channel::setEvents(__uint32_t ev) { events_ = ev; }

__uint32_t Channel::getEvents() { return events_; }

bool Channel::EqualAndUpdateLastEvents() {
    bool ret = (lastEvents_ == events_);
    lastEvents_ = events_;
    return ret;
}

__uint32_t Channel::getLastEvents() { return lastEvents_; }