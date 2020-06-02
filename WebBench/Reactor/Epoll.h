/*
 * @Autor: taobo
 * @Date: 2020-05-29 10:32:48
 * @LastEditTime: 2020-05-29 12:24:28
 * @Description: IO多路复用技术之——epoll
 */ 
#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../base/noncopyable.h"
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

using namespace std;

class Epoll : noncopyable
{
private:
    typedef shared_ptr<Channel> sp_channel;
private:
    static const int MAXFDS = 100000;//100,000
    int epollfd_;
    vector<epoll_event> events_;
    shared_ptr<Channel> chans_[MAXFDS];
    shared_ptr<HttpData> https_[MAXFDS];
    //定时器管理队列
    TimerQueue tq_;
public:
    Epoll();
    ~Epoll();
    void epoll_add(sp_channel req, int timeout);
    void epoll_mod(sp_channel req, int timeout);
    void epoll_del(sp_channel req);
    vector<sp_channel> poll();
    vector<sp_channel> geteventsreq(int event_num);
    void add_timer(sp_channel req,int timeout);
    int getepollfd();
    void handleexpired();
};

