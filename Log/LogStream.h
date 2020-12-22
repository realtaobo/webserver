/*
 * @Autor: taobo
 * @Date: 2020-05-27 21:54:51
 * @LastEditTime: 2020-12-22 13:22:07
 * @Description: 格式化输出，重载了<<运算符
 */
#pragma once
#include <iostream>
#include <string>

#include "../base/noncopyable.h"
#include "LogBuffer.h"

using namespace std;

class LogStream : noncopyable {
 private:
  static const int KMaxNumericSize = 180;
  // typedef FixedBuffer<stream_Size>Buffer;
  // Buffer buffer_;
  string region_;

 public:
  LogStream& operator<<(const void*);
  LogStream& operator<<(char);  // string::+=
  LogStream& operator<<(bool);  // true ? '1' : '0'
  LogStream& operator<<(const char* s);
  LogStream& operator<<(const unsigned char* s);

  LogStream& operator<<(unsigned short);  // std::to_string()
  LogStream& operator<<(unsigned int);
  LogStream& operator<<(unsigned long);
  LogStream& operator<<(unsigned long long);
  LogStream& operator<<(long double);
  LogStream& operator<<(long long);
  LogStream& operator<<(short);
  LogStream& operator<<(int);
  LogStream& operator<<(float);
  LogStream& operator<<(double);
  LogStream& operator<<(long);
  LogStream& operator<<(const string&);

  bool couldAdd(const string& str);
  void append(const string&);
  // const Buffer& buffer();
  void resetBuffer();
  // void region_to_buffer();
  const string& data();
};