/*
 * @Autor: taobo
 * @Date: 2020-05-29 09:28:05
 * @LastEditTime: 2020-12-22 13:22:53
 * @Description: reactor 事件循环发生器
 */
#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "Channel.h"
#include "Epoll.h"

using namespace std;
class Epoll;
class EventLoop {
  typedef function<void()> Functor;

 private:
  bool looping_;              // flag
  shared_ptr<Epoll> poller_;  // IO复用核心类
  mutex mtx_;                 // emmmm....
  bool quit_;                 //控制loop()的生命周期
  int wakefd_;
  shared_ptr<Channel> channel_;  //管理eventfd(起到类似本地套的作用，唤醒线程)
  const pid_t threadId_;  //线程ID，syscall(SYS_gettid)
  void wakeup();          //当线程阻塞在epoll_wait()上时唤醒线程
  vector<Functor> pendingFunctors_;

 public:
  EventLoop();
  ~EventLoop();
  void loop();
  void quit();
  void handleRead();  // wakefd_被EPOLLIN唤醒时调用
  void handleConn() {}
  bool isInLoopThread();                  //判断从属关系
  void shutdown(shared_ptr<Channel> ev);  //关闭某个socket
  void remove_event(shared_ptr<Channel> ev);
  void update_event(shared_ptr<Channel> ev, int timeout = 0);
  void add_event(shared_ptr<Channel> ev, int timeout = 0);

  void queueInLoop(Functor&& cb);
  void doPendingFunctors();
};
