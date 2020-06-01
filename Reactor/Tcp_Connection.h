/*
 * @Autor: taobo
 * @Date: 2020-05-30 16:20:53
 * @LastEditTime: 2020-06-01 18:30:41
 */ 
#pragma once
#include <string>
#include <memory>
#include <functional>
#include "EventLoop.h"
#include "Channel.h"


using namespace std;

enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

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
    function<int()> callback_;
    function<void()> seperate_;
public:
    tcp_connection(EventLoop* p, int fd);
    ~tcp_connection(){ close(cnfd_); }
    EventLoop *getLoop();
    void handleClose();
    void set_event(__uint32_t ev);
    void reg_event();
    string& getinbuffer() { return inBuffer_; }
    void setoutbuffer(const string& str) { outBuffer_ = str; }
    void clearanderror(){ error_=true; inBuffer_.clear();}
    shared_ptr <Channel> getChannel() { return channel_;}
public:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleConn();
    void setcall(function<int()>&& func_){ callback_ = func_;}
    void setseper(function<void()>&& func) { seperate_ = func;}
};
