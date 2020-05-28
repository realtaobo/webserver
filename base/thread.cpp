/*
 * @Autor: taobo
 * @Date: 2020-05-26 18:49:51
 * @LastEditTime: 2020-05-26 21:21:46
 * @Description: file content
 */ 
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "noncopyable.h"
#include "thread.h"

using namespace std;

pid_t gettid() 
{ 
    return static_cast<pid_t>(::syscall(SYS_gettid)); 
}

Thread::Thread(const ThreadFunc& func,const std::string& name)
    : func_(func),name_(name)
{
    if(name.empty()){
        setDefaultName();
    }
}

void Thread::setDefaultName()
{
    time_t rawtime;
    struct tm* ptm;
    rawtime = time(NULL);
    ptm = gmtime(&rawtime);
    char buf[24];
    sprintf(buf,"thread%d%d_%d",ptm->tm_mon,ptm->tm_min,ptm->tm_sec);
    this->name_ = string(buf);
}

int Thread::tid() const
{
    if(!started_)
        return -1;
    return this->tid_;
}

std::string Thread::name() const
{
    return this->name_;
}
void start_thread(std::vector<pid_t>& tid,Thread& obj)
{
    pid_t tmp_id = gettid();
    {
        std::unique_lock<std::mutex> lck(obj.t_mtx_);
        tid.push_back(tmp_id);
        obj.t_cov_.notify_all();
    }
    obj.func_();//重写回调函数可以设置参数

}
void Thread::start()
{
    vector<pid_t> tid;
    std::thread Logthread(start_thread,std::ref(tid),std::ref(*this));
    started_ = true;
    Logthread.detach();
    std::unique_lock<std::mutex> lck(this->t_mtx_);
    t_cov_.wait(lck,[&]{return !tid.empty();});
    if(tid.empty()){
        perror("ipc error!\n");
        exit(0);
    }
    tid_ = tid[0];
}