/*
 * @Autor: taobo
 * @Date: 2020-05-27 18:40:56
 * @LastEditTime: 2020-05-27 21:15:23
 * @Description: file content
 */ 
#pragma once
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"

using namespace std;
//CountDownLatch的主要作用是确保 "线程入口函数“ 确实启动之后主线程才结束Thread的satrt调用
class CountDownLatch: noncopyable
{
private:
    mutex mtx_;
    condition_variable condition_;
    int cnt_;
public:
    explicit CountDownLatch(int cnt);
    void wait();
    void countDown();
    bool blocked();
};