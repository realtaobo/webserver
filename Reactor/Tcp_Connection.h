/*
 * @Autor: taobo
 * @Date: 2020-05-30 16:20:53
 * @LastEditTime: 2020-05-31 19:17:20
 */ 
#pragma once
#include <string>
#include <memory>
#include <functional>
#include "EventLoop.h"
#include "Channel.h"


using namespace std;

enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

typedef function<int()> MyFunc;

class tcp_connection
{
private:
    EventLoop* loop_;
    int cnfd_;
    shared_ptr <Channel> channel_;
    string inBuffer_;   //外部输入缓冲区
    string outBuffer_;  //外部输出缓冲区
    ConnectionState conn_state_;
    bool error_;

    //callback()
    MyFunc parse_URI;
    MyFunc parse_Headers;
    MyFunc analysisRequest;
public:
    tcp_connection(EventLoop* p, int fd);
    ~tcp_connection(){ close(cnfd_); }
    EventLoop *getLoop();
    void handleClose();
    void set_event(__uint32_t ev);
    void reg_event();
    string& getinbuffer() { return inBuffer_; }
    void setoutbuffer(const string& str) { outBuffer_ = str; }
public:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleConn();
    void seturi(MyFunc&& func_){ parse_URI = func_;}
    void setheader(MyFunc&& func_) { parse_Headers = func_; }
    void setanaly(MyFunc&& func_) { analysisRequest = func_; }
};
