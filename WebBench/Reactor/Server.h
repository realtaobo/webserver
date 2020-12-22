/*
 * @Autor: taobo
 * @Date: 2020-05-30 13:19:23
 * @LastEditTime: 2020-05-30 14:43:32
 * @Description: tcp_server
 */
#pragma once
#include <memory>

#include "Channel.h"
#include "EventLoop.h"
#include "ThreadPool.h"

using namespace std;

class Server {
 private:
  bool started_;
  EventLoop* baseloop_;
  int port_;
  int listen_fd_;
  shared_ptr<Channel> acceptchannel_;
  int threadnums_;
  unique_ptr<ThreadPool> threadpool_;
  static const int MAXFDS = 100000;  //系统最大并发连接数
 public:
  Server(EventLoop* base, int port, int nums = 1);
  ~Server() = default;
  void start();
  void handNewConn();
  void handThisConn();
};
