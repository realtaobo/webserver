/*
 * @Autor: taobo
 * @Date: 2020-05-29 10:48:18
 * @LastEditTime: 2020-05-29 13:02:47
 * @Description: file content
 */ 
#include "Epoll.h"
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../base/CurrentThread.h"
#include "../Log/Logger.h"
#include "../base/noncopyable.h"
#include "Channel.h"
#include "HttpData.h"


const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

using namespace std;
Epoll::Epoll():epollfd_(epoll_create1(EPOLL_CLOEXEC)),events_(EVENTSNUM){}

Epoll::~Epoll(){}
void Epoll::add_timer(sp_channel req,int timeout)
{
    shared_ptr<HttpData> t = req->getHolder();  //weak_ptr观察的Http对象
    if(t)
        tq_.push(t,timeout);
    else
        LOG << "timer add fail";
}
void Epoll::epoll_add(sp_channel req, int timeout)
{
    int fd = req->getFd();
    if(timeout > 0)
    {
        add_timer(req,timeout);
        https_[fd] = req->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = req->getEvents();
    req->EqualAndUpdateLastEvents();
    chans_[fd] = req;   //save channel
    if(epoll_ctl(epollfd_,EPOLL_CTL_ADD, fd, &event) < 0)
    {
        LOG << CurrentThread::t_cachedTid <<": epoll_add error" ;
        chans_[fd].reset();
    }
}
void Epoll::epoll_mod(sp_channel req, int timeout)
{
    
}
void Epoll::epoll_del(sp_channel req)
{

}
vector<shared_ptr<Channel>> Epoll::poll()
{

}
vector<shared_ptr<Channel>> Epoll::geteventsreq(int event_num)
{

}

int Epoll::getepollfd()
{

}
void Epoll::handleexpired()
{
    
}