/*
 * @Autor: taobo
 * @Date: 2020-05-29 21:58:07
 * @LastEditTime: 2020-05-29 22:12:27
 */
#pragma once
#include <condition_variable>
#include <mutex>

#include "../base/noncopyable.h"
#include "../base/thread.h"
#include "EventLoop.h"
using namespace std;

class EventLoopThread : noncopyable {
 private:
  void threadFunc();  //线程入口函数
  EventLoop* loop_;
  Thread thread_;
  mutex mtx_;  // emm...
  condition_variable cond_;

 public:
  EventLoopThread();
  ~EventLoopThread();
  EventLoop* startLoop();
};