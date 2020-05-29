/*
 * @Autor: taobo
 * @Date: 2020-05-29 11:07:01
 * @LastEditTime: 2020-05-29 11:59:29
 * @Description: 定时器
 */ 
#include <queue>
#include <deque>
#include <memory>
#include <sys/time.h>
#include <unistd.h>

#include "Timer.h"

using namespace std;

Timer::Timer(shared_ptr<HttpData> request ,int timeout):deleted_(false),sp_httpdata(request)
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    expiredtime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

Timer::Timer(const Timer& t):sp_httpdata(t.sp_httpdata),expiredtime_(0){}

Timer::~Timer()
{
    //if(sp_httpdata)
        //sp_httpdata->handleClose();
}

void Timer::update(int timeout)
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    expiredtime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;    
}
bool Timer::isvalid()
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    size_t tmp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if(tmp<expiredtime_)
        return true;
    else
    {
        setdeleted();
        return false;
    }
}
void Timer::clearreq()
{
    sp_httpdata.reset();
    setdeleted();
}
void Timer::setdeleted(){ deleted_ = true; }
bool Timer::isdeleted(){ return deleted_; }
size_t Timer::getexptime() {return expiredtime_;}

void TimerQueue::push(shared_ptr<HttpData> sphttpdata,int timeout)
{
    sptimer node(new Timer(sphttpdata,timeout));
    timerqueue.push(node);
    //sphttpdata->linkTimer(new_node);
}
void TimerQueue::handleexpired()
{
    while (!timerqueue.empty())
    {
        sptimer tmp = timerqueue.top();
        if(tmp->isdeleted())
            timerqueue.pop();
        else if(tmp->isvalid())
            timerqueue.pop();
        else
            break;
    }
    
}