/*
 * @Autor: taobo
 * @Date: 2020-05-30 19:37:07
 * @LastEditTime: 2020-05-30 19:54:45
 */ 
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Tcp_Connection.h"
#include "EventLoop.h"
#include "Timer.h"
#include "HttpData.h"

using namespace std;

HttpData::HttpData(EventLoop* p, int fd):
tcp_server(new tcp_connection(p,fd)),
method_(METHOD_GET),
httpversion_(HTTP_11),
readpos_(0),
state_(STATE_PARSE_URI),
hState_(H_START),
keepalive_(false)
{}

EventLoop* HttpData::getLoop()
{
    return tcp_server->getLoop();
}

void HttpData::handleClose()
{
    tcp_server->handleClose();
}

void HttpData::newEvent()
{
    tcp_server->set_event(EPOLLIN|EPOLLET|EPOLLONESHOT);
    tcp_server->reg_event();
}

void HttpData::seperateTimer() {
    if (timer_.lock()) {
        shared_ptr<Timer> my_timer(timer_.lock());
        my_timer->clearreq();
        timer_.reset();
    }
}

void HttpData::reset()
{
    filename_.clear();
    path_.clear();
    readpos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
    keepalive_ = false;
    this->seperateTimer();
}