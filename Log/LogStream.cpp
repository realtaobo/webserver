/*
 * @Autor: taobo
 * @Date: 2020-05-27 23:14:37
 * @LastEditTime: 2020-05-28 12:14:23
 * @Description: Logstream的实现
 */
#include "LogStream.h"

#include <cstring>
#include <iostream>
#include <string>

using namespace std;

const string& LogStream::data() { return region_; }

bool LogStream::couldAdd(const string& str) {
  if (str.length() + region_.length() > KMaxNumericSize) {
    return false;
  }
  return true;
}

void LogStream::append(const string& str) {
  if (couldAdd(str)) {
    region_ += str;
  }
}

// const LogStream::Buffer& LogStream::buffer()
// {
//     return buffer_;
// }

void LogStream::resetBuffer() { region_.clear(); }

LogStream& LogStream::operator<<(const void* s) { return *this; }

LogStream& LogStream::operator<<(char c)  // string::+=
{
  if (region_.length() + 1 <= KMaxNumericSize) {
    region_ += c;
  }
  return *this;
}

LogStream& LogStream::operator<<(bool v)  // true ? '1' : '0'
{
  if (region_.length() + 1 <= KMaxNumericSize) {
    region_ += (v ? '1' : '0');
  }
  return *this;
}

LogStream& LogStream::operator<<(const char* s) {
  if (region_.length() + strlen(s) <= KMaxNumericSize) {
    region_.append(s);
  }
  return *this;
}

LogStream& LogStream::operator<<(const unsigned char* s) {
  return operator<<(reinterpret_cast<const char*>(s));
}

LogStream& LogStream::operator<<(unsigned short num)  // std::to_string()
{
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}

LogStream& LogStream::operator<<(unsigned int num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(unsigned long num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(unsigned long long num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(long double num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(long long num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(short num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(int num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(float num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(double num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(long num) {
  string tmp = to_string(num);
  if (couldAdd(tmp)) {
    region_ += tmp;
  }
  return *this;
}
LogStream& LogStream::operator<<(const string& str) {
  if (couldAdd(str)) {
    region_ += str;
  }
  return *this;
}