/*
 * @Autor: taobo
 * @Date: 2020-05-26 14:53:52
 * @LastEditTime: 2020-06-01 18:00:40
 * @Description: 负责启动一个log线程，专门用来将log写入LogFile，应用了“双缓冲技术”
 *              也即日志的前后端分离技术
 */ 
#pragma once
#include <memory>
#include <condition_variable>

#include "LogBuffer.h"
#include "../base/noncopyable.h"
#include "../base/thread.h"
#include "../base/CountDownLatch.h"


using namespace std;

class AsyncLogging: noncopyable
{
private:
    typedef FixedBuffer<s_BufferSize> Buffer;
    typedef shared_ptr<Buffer> BufferPtr;
    typedef vector<BufferPtr> BufferVector;
    //私有变量
    int flushInterval_;
    bool running_ = false;
    string basename_;
    Thread thread_;
    mutex mtx_;
    condition_variable cov_;    
    BufferPtr currentBuffer_;//前端缓冲区1
    BufferPtr nextBuffer_;  //前端缓冲区2，当1满了后相互调换
    BufferVector buffers_;  //后端，负责向磁盘写
    CountDownLatch latch_;

public:
    AsyncLogging(const string& basename, int flushInterval = 2);
    ~AsyncLogging() = default;
    void append(const string& str);
    void start();
    void threadFunc();
};
