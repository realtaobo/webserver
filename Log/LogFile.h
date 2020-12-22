/*
 * @Autor: taobo
 * @Date: 2020-05-26 14:15:28
 * @LastEditTime: 2020-05-27 21:04:51
 * @Description: 提供了一种简单的写入策略，当日志记录数达到flushEveryN_
 *      时调用底层类AppendFile类的flush函数刷新
 */
#pragma once
#include <memory>
#include <mutex>
#include <string>

#include "../base/noncopyable.h"
#include "FileUtil.h"

using namespace std;

class LogFile : noncopyable {
 private:
  string filename_;
  const int flushEveryN_;
  int cnt_;  //当cnt==flushEveryN_时将内存日志刷到磁盘上
  unique_ptr<mutex> mtx_;        //保护日志锁
  unique_ptr<AppendFile> file_;  //底层类对象
  void append_unlocked(const char* logline, size_t len);

 public:
  LogFile(const string& basename, int flushlen = 512);
  ~LogFile() = default;
  void append(const char* logline, size_t len);
  void flush();
};
