/*
 * @Autor: taobo
 * @Date: 2020-05-29 10:35:13
 * @LastEditTime: 2020-12-22 13:24:10
 * @Description: 定时器，基于priority_queue实现
 */
#pragma once
#include <unistd.h>

#include <deque>
#include <memory>
#include <queue>

#include "../base/noncopyable.h"
class HttpData;
using namespace std;

class Timer {
 private:
  bool deleted_;
  size_t expiredtime_;
  shared_ptr<HttpData> sp_httpdata;

 public:
  //设计绝对时间为((now.tv_sec % 10000) * 1000) + (now.tv_usec /
  // 1000)，则expiredtime_ = *+timeout
  Timer(shared_ptr<HttpData> request, int timeout);
  ~Timer();
  Timer(const Timer& t);
  void update(int timeout);
  bool isvalid();
  void clearreq();
  void setdeleted();
  bool isdeleted();
  size_t getexptime();
};

struct Compare  //为优先队列设计的仿函数
{
  bool operator()(shared_ptr<Timer>& a, shared_ptr<Timer>& b) {
    return a->getexptime() > b->getexptime();
  }
};

class TimerQueue {
 private:
  typedef shared_ptr<Timer> sptimer;
  priority_queue<sptimer, vector<sptimer>, Compare> timerqueue;

 public:
  TimerQueue() = default;
  ~TimerQueue() = default;
  void push(shared_ptr<HttpData> sphttpdata, int timeout);
  void handleexpired();
};