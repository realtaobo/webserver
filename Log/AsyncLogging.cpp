/*
 * @Autor: taobo
 * @Date: 2020-05-26 21:58:25
 * @LastEditTime: 2020-06-01 10:53:01
 * @Description: 该类负责维护日志线程的搭建与维护，AsyncLogging::threadFunc()函数是日志缓冲区的核心设计
 */ 
#include "../base/CountDownLatch.h"
#include "AsyncLogging.h"
#include "LogFile.h"
#include <iostream>
#include <functional>
#include <vector>
#include <assert.h>

using namespace std;

//void assert(int expression);
AsyncLogging::AsyncLogging(const string& basename, int flushInterval)
:   flushInterval_(flushInterval),basename_(basename),
    thread_(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
    currentBuffer_(new Buffer), nextBuffer_(new Buffer),
    buffers_(), latch_(1)
{
    buffers_.reserve(16);   //16缓冲区
}

void AsyncLogging::append(const string& str)  //日志前台入队
{
    unique_lock<mutex> lck(mtx_);   //加锁，保证线程安全
    if(currentBuffer_->avail()){
        currentBuffer_->append(str);
    }else{
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset(); //释放空间
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);//所有权转移
        }else{
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(str);
        cov_.notify_all();
    }
}

void AsyncLogging::start()
{
    running_ = true;
    thread_.start();
    latch_.wait();
}

void AsyncLogging::threadFunc()
{
    latch_.countDown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    BufferVector bufferToWrite;
    bufferToWrite.reserve(16);
    while (this->running_)
    {
        //cout<<"start.....running...."<<endl;
        assert(newBuffer2 && newBuffer2->length()==0);
        assert(newBuffer1 && newBuffer1->length()==0);
        assert(bufferToWrite.empty());
        {//使得锁的粒度尽可能的小
            /*后台日志缓冲区设计措施*/
            unique_lock<mutex> lck(mtx_);
            while(buffers_.empty()){
                cov_.wait(lck);
            }
            //下面的措施仅发生缓冲区指针的对换，不涉及缓冲区拷贝，效率应当是可以的
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();
            currentBuffer_ = move(newBuffer1);
            bufferToWrite.swap(buffers_);
            if(!nextBuffer_){
                nextBuffer_ = move(newBuffer2);
            }
        }
        //若一切顺利，到此处时bufferToWrite应不为空
        assert(!bufferToWrite.empty());
        if(bufferToWrite.size()>16)
        {//16*400 = 64000*10 = 640000bytes 
            bufferToWrite.erase(bufferToWrite.begin()+4,bufferToWrite.end());
        }
        //LogFile output(basename_)借助该对象将日志写入磁盘
        //cout<<"write.....running...."<<endl;
        for(size_t i=0;i<bufferToWrite.size(); ++i)
        {
            bufferToWrite[i]->send_data(output);
        }
        //cout<<"over.....running...."<<endl;
        bufferToWrite.resize(2);
        if(!newBuffer1)
        {
            newBuffer1 = bufferToWrite.back();
            bufferToWrite.pop_back();
            newBuffer1->bzero();
        }
        if(!newBuffer2)
        {
            newBuffer2 = bufferToWrite.back();
            bufferToWrite.pop_back();
            newBuffer2->bzero();
        }
        bufferToWrite.clear();
        output.flush();
    }
    output.flush();
}