/*
 * @Autor: taobo
 * @Date: 2020-05-26 16:52:32
 * @LastEditTime: 2020-12-22 13:25:35
 * @Description: 对C++11中thread的简单封装，保存一些常见值，提高响应速度
 */
#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "noncopyable.h"
//默认对线程采取的是detach策略，因为后台程序不会涉及线程安全问题
class Thread : noncopyable {
 public:
  typedef std::function<void()> ThreadFunc;
  bool started_ = false;  // start flag
  // bool joined_ = false;   //join flag
  ThreadFunc func_;   // thread function
  std::string name_;  // thrad name
  pid_t tid_ = -1;    // thread_id
  std::mutex t_mtx_;
  std::condition_variable t_cov_;  // mutex与condition，总会用到吧
 public:
  explicit Thread(const ThreadFunc&, const std::string& name = std::string());
  ~Thread() = default;
  void start();
  // int join();
  // bool started() const;
  int tid() const;
  std::string name() const;
  void setDefaultName();
};
