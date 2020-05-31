/*
 * @Autor: taobo
 * @Date: 2020-05-30 16:53:14
 * @LastEditTime: 2020-05-31 22:55:22
 */ 
#include "Tcp_Connection.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "Channel.h"
#include "../base/Package.h"
#include "../Log/Logger.h"

#include <string>
#include <cstring>
#include <functional>

using namespace std;

tcp_connection::tcp_connection(EventLoop* base,int fd):
loop_(base),
cnfd_(fd),
channel_(new Channel(base,fd)),
conn_state_(H_CONNECTED),
error_(false)
{
    channel_->setReadHandler(std::bind(&tcp_connection::handleRead,this));
    channel_->setWriteHandler(std::bind(&tcp_connection::handleWrite,this));
    channel_->setErrorHandler(std::bind(&tcp_connection::handleError,this));
    channel_->setConnHandler(std::bind(&tcp_connection::handleConn,this));
}
void tcp_connection::handleClose()
{
    conn_state_ = H_DISCONNECTED;
    loop_->remove_event(channel_);
}
EventLoop* tcp_connection::getLoop()
{
    return loop_;
}
void tcp_connection::set_event(__uint32_t ev)
{
    channel_->setEvents(ev);
}
void tcp_connection::reg_event()
{
    loop_->add_event(channel_,2000);
}
// void tcp_connection::handleError()
// {
//     string short_msg(" Bad Request");
//     char send_buff[4096];
//     string body_buff, header_buff;
//     //http 响应报文主体
//     body_buff += "<html><title>哎~出错了</title>";
//     body_buff += "<body bgcolor=\"ffffff\">";
//     body_buff += to_string(400) + short_msg;
//     body_buff += "<hr><em> C++11 WebServer</em>\n</body></html>";
//     //http 响应报文头部
//     header_buff += "HTTP/1.1 " + to_string(400) + short_msg + "\r\n";
//     header_buff += "Content-Type: text/html\r\n";
//     header_buff += "Connection: Close\r\n";
//     header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
//     header_buff += "Server: C++11 WebServer\r\n";
//     header_buff += "\r\n";
//     //错误处理而已，就不处理writen返回值了
//     sprintf(send_buff, "%s", header_buff.c_str());
//     writen(cnfd_, send_buff, strlen(send_buff));
//     sprintf(send_buff, "%s", body_buff.c_str());
//     writen(cnfd_, send_buff, strlen(send_buff));
// }

// void tcp_connection::handleRead()
// {
//     bool zero = false;
//     int read_sum = readn(cnfd_,inBuffer_,zero);
//     LOG << "Request: " << inBuffer_ ;
//     if(conn_state_ != H_CONNECTED)
//     {
//         inBuffer_.clear();
//         return;
//     }
//     if(read_sum < 0 || zero)
//     {
//         perror("coonnect_fd read error");
//         error_ = true;
//         conn_state_ = H_DISCONNECTED;
//         handleError();
//         return;
//     }//else if(zero)//此时表示连接出现了问题，可能是连接断开或者其他...
//     // {
//     //     conn_state_ = H_DISCONNECTING;
//     // }
//     int res = callback_();
//     if(res == 0 && conn_state_ == H_CONNECTED)
//     {
//         if(outBuffer_.size() > 0){
//             handleWrite();
//         }
//         clearanderror();
//     }
// }

// void tcp_connection::handleWrite()
// {
//     if(!error_ && conn_state_ != H_DISCONNECTED)
//     {
//         __uint32_t &ev = channel_->getEvents();
//         if(writen(cnfd_,outBuffer_) < 0)
//         {
//             perror("tcp_cnn write error");
//             ev = 0;
//             error_ = true;
//         }
//         if(outBuffer_.size() > 0)   //未写完则注册写事件，下次继续写
//         {    
//             ev |= EPOLLOUT;
//             reg_event();
//         }
//     }
// }

void tcp_connection::handleRead(){
    bool zero = false;
    int read_sum = readn(cnfd_,inBuffer_,zero);
    if(read_sum < 0 || zero)
    {
        perror("coonnect_fd read error");
        error_ = true;
        conn_state_ = H_DISCONNECTED;
        handleError();
        return;
    }
    LOG << "Request: " << inBuffer_ ;
    if(conn_state_ != H_CONNECTED)
    {
        inBuffer_.clear();
        return;
    }
    for(char& c:inBuffer_){
        outBuffer_.push_back(c);
    }
    inBuffer_.clear();
    reg_event();
}
void tcp_connection::handleConn(){}
void tcp_connection::handleWrite(){
    writen(cnfd_,outBuffer_);
    outBuffer_.clear();
}
void tcp_connection::handleError()
{
    ;
}