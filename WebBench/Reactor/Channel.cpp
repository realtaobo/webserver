/*
 * @Autor: taobo
 * @Date: 2020-05-29 09:03:26
 * @LastEditTime: 2020-12-22 13:25:54
 */
#include "Channel.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <functional>
#include <memory>

using namespace std;

Channel::Channel(EventLoop *loop) : loop_(loop) {}

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd) {}

Channel::~Channel() {}

void Channel::setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }

shared_ptr<HttpData> Channel::getHolder() {
  shared_ptr<HttpData> ret(holder_.lock());
  return ret;
}

int Channel::getFd() { return fd_; }

void Channel::setFd(int fd) { fd_ = fd; }

void Channel::handleRead() {
  if (readHandler_) {
    readHandler_();
  }
}

void Channel::handleWrite() {
  if (writeHandler_) {
    writeHandler_();
  }
}

void Channel::handleConn() {
  if (connHandler_) {
    connHandler_();
  }
}
void Channel::handleError(int fd, int err_num, std::string short_msg) {}
void Channel::handleEvents() {
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

void Channel::setRevents(__uint32_t ev) { revents_ = ev; }

void Channel::setEvents(__uint32_t ev) { events_ = ev; }

__uint32_t &Channel::getEvents() { return events_; }

bool Channel::EqualAndUpdateLastEvents() {
  bool ret = (lastEvents_ == events_);
  lastEvents_ = events_;
  return ret;
}

__uint32_t Channel::getLastEvents() { return lastEvents_; }
