/*
 * @Autor: taobo
 * @Date: 2020-05-29 10:48:18
 * @LastEditTime: 2020-05-29 21:05:26
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
    if (timeout > 0) add_timer(req, timeout);
    int fd = req->getFd();
    if(!req->EqualAndUpdateLastEvents()) {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = req->getEvents();
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
            LOG << CurrentThread::t_cachedTid <<": epoll_mod error" ;
            chans_[fd].reset();
        }
    }
}
void Epoll::epoll_del(sp_channel req)
{
    int fd = req->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = req->getLastEvents();
    if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,fd,&event) < 0)
        LOG << CurrentThread::t_cachedTid <<": epoll_del error" ;
    chans_[fd].reset();
    https_[fd].reset();
}
vector<shared_ptr<Channel>> Epoll::geteventsreq(int event_num)
{
    vector<sp_channel> req_data;
    for(int i=0;i<event_num;++i)
    {
        int fd = events_[i].data.fd;
        sp_channel tmp = chans_[fd];
        if(tmp)
        {
            tmp->setRevents(events_[i].events);
            tmp->setEvents(0);
            req_data.push_back(tmp);
        }else
        {
            LOG << "shared_ptr<Channel> is invalid" ;
        }
    }
    return req_data;
}
vector<shared_ptr<Channel>> Epoll::poll()
{
    int event_cnt = 
            epoll_wait(epollfd_,&(*events_.begin()),events_.size(),EPOLLWAIT_TIME);
    vector<shared_ptr<Channel>> req_data= geteventsreq(event_cnt);
    return req_data;
}

int Epoll::getepollfd()
{
    return epollfd_;
}
void Epoll::handleexpired()
{
    tq_.handleexpired();
}