/*
 * @Autor: taobo
 * @Date: 2020-05-26 18:49:51
 * @LastEditTime: 2020-05-28 22:56:24
 * @Description: file content
 */
#include "thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "CurrentThread.h"
#include "noncopyable.h"

using namespace std;

namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";
}  // namespace CurrentThread

Thread::Thread(const ThreadFunc& func, const std::string& name)
    : func_(func), name_(name) {
  if (name.empty()) {
    setDefaultName();
  }
}

void Thread::setDefaultName() {
  time_t rawtime;
  struct tm* ptm;
  rawtime = time(NULL);
  ptm = gmtime(&rawtime);
  char buf[24];
  sprintf(buf, "thread-%d", ptm->tm_mon);
  this->name_ = string(buf);
}

int Thread::tid() const {
  if (!started_) return -1;
  return this->tid_;
}

std::string Thread::name() const { return this->name_; }
void start_thread(std::vector<pid_t>& tid, Thread& obj) {
  pid_t tmp_id = CurrentThread::tid();
  {
    std::unique_lock<std::mutex> lck(obj.t_mtx_);
    tid.push_back(tmp_id);
    obj.t_cov_.notify_all();
  }
  CurrentThread::t_threadName =
      (obj.name_).empty() ? "Thread" : (obj.name_).c_str();
  obj.func_();  //重写回调函数可以设置参数
}
void Thread::start() {
  vector<pid_t> tid;
  std::thread mythread(start_thread, std::ref(tid), std::ref(*this));
  started_ = true;
  mythread.detach();
  std::unique_lock<std::mutex> lck(this->t_mtx_);
  t_cov_.wait(lck, [&] { return !tid.empty(); });
  if (tid.empty()) {
    perror("ipc error!\n");
    exit(0);
  }
  tid_ = tid[0];
}