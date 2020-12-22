/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:57:19
 * @LastEditTime: 2020-12-22 13:21:34
 * @Description: 最底层的文件类定义，封装了日志文件的打开、写入，并在
 * 类析构的时候关闭文件，成员函数append直接向文件写
 */
#pragma once
#include <stdio.h>

#include <string>

#include "../base/noncopyable.h"

//该类提供了一些基本的写入接口，但没有提供合理的写入策略与刷新策略
class AppendFile : noncopyable {
 private:
  size_t write(const char* logline, size_t len);
  FILE* fp_;
  char buffer_[64 * 1024];  // 64KB
 public:
  explicit AppendFile(const std::string& filename);
  ~AppendFile();
  // append 真正向文件写数据
  void append(const char* logline, const size_t len);
  void flush();
};
