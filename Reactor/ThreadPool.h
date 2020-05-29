/*
 * @Autor: taobo
 * @Date: 2020-05-29 22:29:28
 * @LastEditTime: 2020-05-29 22:42:03
 * @Description: 线程池的构建，本质就是一个个的eventloopthread 的集合
 */ 
#pragma once
#include <memory>
#include <vector>
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "../Log/Logger.h"
#include "../base/noncopyable.h"

using namespace std;

class ThreadPool : noncopyable
{
private:
    typedef shared_ptr<EventLoopThread> sp_evthread;
    EventLoop* baseloop_;
    bool started_;
    int numThreads_;
    int next_; //将来将以轮询的方式为每个线程分发fd
    vector<sp_evthread> threads_;
    vector<EventLoop*> loops_;
public:
    ThreadPool(EventLoop* base, int nums);
    ~ThreadPool(){LOG <<"~ThreadPool()";}
    void start();
    EventLoop* getNextLoop();
};