/*
 * @Autor: taobo
 * @Date: 2020-05-28 22:13:25
 * @LastEditTime: 2020-05-31 23:43:06
 * @Description: 定义一个有意思的命名空间CurrentThread ，期间使用了一个GCC
 * 变量__thread
 *  对于该类型变量创建线程时会使得每一个线程都有一份独立的实体，当然其只能修饰POD类型变量
 */
#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "noncopyable.h"

using namespace std;
pid_t gettid();
namespace CurrentThread {
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;
inline void cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
    t_tidStringLength =
        snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

inline int tid() {
  if (t_cachedTid == 0) {
    cacheTid();
  }
  return t_cachedTid;
}

inline const char* tidString() { return t_tidString; }

inline int tidStringLength() { return t_tidStringLength; }

inline const char* name() { return t_threadName; }
}  // namespace CurrentThread
