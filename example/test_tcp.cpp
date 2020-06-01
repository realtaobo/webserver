/*
 * @Autor: taobo
 * @Date: 2020-06-01 19:31:47
 * @LastEditTime: 2020-06-01 19:47:59
 * @Description: 观察作为一个简单的echo服务器
 */ 
#include <iostream>
#include <getopt.h>
#include <string>
#include  "../Log/Logger.h"
#include  "../Reactor/EventLoop.h"
#include  "../Reactor/Server.h"
using namespace std;
/*下面几个函数是echo server 的几个回调函数*/
// void tcp_connection::handleRead(){
//     bool zero = false;
//     int read_sum = readn(cnfd_,inBuffer_,zero);
//     if(read_sum < 0 || zero)
//     {
//         error_ = true;
//         conn_state_ = H_DISCONNECTED;
//         handleError();
//         return;
//     }
//     LOG << "Request: " << inBuffer_ ;
//     if(conn_state_ != H_CONNECTED)
//     {
//         inBuffer_.clear();
//         return;
//     }
//     for(char& c:inBuffer_){
//         outBuffer_.push_back(c+1);
//     }
//     inBuffer_.clear();
//     handleWrite();
// }
// void tcp_connection::handleConn()
// {
//     seperate_();
//     //__uint32_t &events = channel_->getEvents();
//     if(!error_ && conn_state_ == H_CONNECTED)//一切正常的话
//     {
//         //cout<<__FILE__<<" "<<__LINE__<<endl;
//         set_event(EPOLLET|EPOLLIN);
//         loop_->update_event(channel_,2000);
//     }else{
//         cout<<__FILE__<<" "<<__LINE__<<endl;
//         conn_state_ = H_DISCONNECTED;
//     }
// }
// void tcp_connection::handleWrite(){
//     writen(cnfd_,outBuffer_);
//     outBuffer_.clear();
// }
// void tcp_connection::handleError()
// {
//     ;
// }

int main_2(int argc, char* argv[])
{
    int threadNum = 4;
    int port = 826;
    string logPath = "./webserver.log";
    Logger::setLogFileName(logPath);
    
    EventLoop mainloop;
    Server myserver(&mainloop,port,threadNum);
    //cout<<__FILE__<<"  "<<__LINE__<<endl;
    myserver.start();
    mainloop.loop();
    return 0;
}

输出日志（部分）：
2020-06-01 19:42:01
New connection from 192.168.160.1:49389 -- /home/go/webserver/Reactor/Server.cpp:62
2020-06-01 19:42:03
Request: re
 -- /home/go/webserver/Reactor/Tcp_Connection.cpp:136
2020-06-01 19:42:34
New connection from 127.0.0.1:49392 -- /home/go/webserver/Reactor/Server.cpp:62
2020-06-01 19:42:36
Request: er
 -- /home/go/webserver/Reactor/Tcp_Connection.cpp:136
