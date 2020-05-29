/*
 * @Autor: taobo
 * @Date: 2020-05-29 09:28:05
 * @LastEditTime: 2020-05-29 21:37:46
 * @Description: reactor 事件循环发生器
 */ 
#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "Channel.h"
#include "Epoll.h"

extern __thread EventLoop* t_loopInThisThread;

using namespace std;

class EventLoop
{
    typedef function<void()> Functor;
private:
    bool looping_;  //flag
    shared_ptr<Epoll> poller_;      //IO复用核心类
    mutex   mtx_;   //emmmm....
    shared_ptr<Channel> channel_;   //管理eventfd(起到类似本地套的作用，唤醒线程)
    int wakefd_;    //emmm , 上面channel_管理的fd 就是我
    const pid_t threadId_;  //线程ID，syscall(SYS_gettid)
    bool quit_;      //控制loop()的生命周期
    void wakeup(); //当线程阻塞在epoll_wait()上时唤醒线程
    
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void handleRead();  //wakefd_被EPOLLIN唤醒时调用
    bool isInLoopThread();//判断从属关系
    void shutdown(shared_ptr<Channel> ev);//关闭某个socket
    void remove_event(shared_ptr<Channel> ev);
    void update_event(shared_ptr<Channel> ev, int timeout = 0);
    void add_event(shared_ptr<Channel> ev, int timeout = 0);
};
