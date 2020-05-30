/*
 * @Autor: taobo
 * @Date: 2020-05-30 16:20:53
 * @LastEditTime: 2020-05-30 17:11:18
 */ 
#pragma once
#include <string>
#include <memory>
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
public:
    tcp_connection(EventLoop* base,int fd);
    void handleRead();
    void handleWrite();
    void handleError();
    void handleConn();
};
