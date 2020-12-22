/*
 * @Autor: taobo
 * @Date: 2020-05-29 22:03:46
 * @LastEditTime: 2020-12-22 13:23:09
 */
#include "EventLoopThread.h"

#include <condition_variable>
#include <mutex>

#include "../base/thread.h"
#include "EventLoop.h"
using namespace std;

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread") {}

EventLoopThread::~EventLoopThread() {
  if (loop_ != nullptr) loop_->quit();
}

EventLoop* EventLoopThread::startLoop() {
  thread_.start();
  {
    unique_lock<mutex> lck(mtx_);
    while (loop_ == nullptr) cond_.wait(lck);
  }
  return loop_;
}
void EventLoopThread::threadFunc() {
  EventLoop loop;
  {
    unique_lock<mutex> lck(mtx_);
    loop_ = &loop;
    cond_.notify_all();
  }
  loop.loop();
  loop_ = nullptr;
}