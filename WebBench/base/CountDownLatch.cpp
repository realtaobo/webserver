/*
 * @Autor: taobo
 * @Date: 2020-05-27 19:04:46
 * @LastEditTime: 2020-05-27 21:15:13
 * @Description: file content
 */ 
#include <mutex>
#include <condition_variable>

#include "CountDownLatch.h"

using namespace std;

CountDownLatch::CountDownLatch(int cnt):cnt_(cnt){}

void CountDownLatch::wait()
{
    unique_lock<mutex> lck(this->mtx_);
    condition_.wait(lck,[&]{return !blocked();});
}

bool CountDownLatch::blocked()
{
    return cnt_>0;
}

void CountDownLatch::countDown()
{
    unique_lock<mutex> lck(this->mtx_);
    --cnt_;
    if(cnt_==0) this->condition_.notify_all();
}
