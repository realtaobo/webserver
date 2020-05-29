/*
 * @Autor: taobo
 * @Date: 2020-05-29 19:26:29
 * @LastEditTime: 2020-05-29 21:38:51
 */ 
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include <sys/eventfd.h>
#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "../base/CurrentThread.h"
#include "../base/Package.h"
#include "../Log/Logger.h"

using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd() 
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG << "Failed in eventfd";
        abort();
    }
    return evtfd;
}
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = writen(wakefd_, (char*)(&one), sizeof one);
    if (n != sizeof one) {
        LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}
EventLoop::EventLoop():looping_(false),
poller_(new Epoll()),
mtx_(), quit_(false),
wakefd_(createEventfd()),
threadId_(CurrentThread::tid()),
channel_(new Channel(this,wakefd_))
{
    t_loopInThisThread = this;
    channel_->setEvents(EPOLLIN);
    channel_->setReadHandler(bind(&EventLoop::handleRead,this));
    poller_->epoll_add(channel_, 0);
}
EventLoop::~EventLoop()
{
    close(wakefd_);
    t_loopInThisThread = NULL;
}
bool EventLoop::isInLoopThread()//判断从属关系
{
    return threadId_ == CurrentThread::tid();
}
void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}
typedef shared_ptr<Channel> sp_channel;
void EventLoop::loop()
{
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    vector<sp_channel> ret;
    while (!quit_)
    {
        ret.clear();
        ret = poller_->poll();
        for(auto& it: ret) it->handleEvents();
        poller_->handleexpired();
    }
    looping_ = false;
}

void EventLoop::handleRead()  //wakefd_被EPOLLIN唤醒时调用
{
    uint64_t one = 1;
    ssize_t n = readn(wakefd_,&one, sizeof one);
    if(n != sizeof one)
        LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    //eventLOOP里面在搞个东西进行通信
}

void EventLoop::shutdown(shared_ptr<Channel> ev)//关闭某个socket
{
    shutDownWR(ev->getFd());
}
void EventLoop::remove_event(shared_ptr<Channel> ev)
{
    poller_->epoll_del(ev);
}
void EventLoop::update_event(shared_ptr<Channel> ev, int timeout)
{
    poller_->epoll_mod(ev,timeout);
}
void EventLoop::add_event(shared_ptr<Channel> ev,int timeout)
{
    poller_->epoll_add(ev,timeout);
}


