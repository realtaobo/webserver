/*
 * @Autor: taobo
 * @Date: 2020-05-30 13:32:34
 * @LastEditTime: 2020-06-01 17:47:06
 */ 
#include <memory>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Server.h"
#include "Channel.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "HttpData.h"
#include "../base/Package.h"

using namespace std;

Server::Server(EventLoop* base, int port, int nums): started_(false),
baseloop_(base),
port_(port), 
listen_fd_(socket_bind_listen(port)),
acceptchannel_(new Channel(base,listen_fd_)),
threadnums_(nums),
threadpool_(new ThreadPool(base,nums))
{
    handle_for_sigpipe();
    if(setSocketNonBlocking(listen_fd_) < 0)
    {
        perror("set non_sock error\n");
        abort();
    }
}

void Server::start()
{
    threadpool_->start();
    acceptchannel_->setEvents(EPOLLET | EPOLLIN);
    acceptchannel_->setReadHandler(std::bind(&Server::handNewConn,this));
    acceptchannel_->setConnHandler(std::bind(&Server::handThisConn,this));
    baseloop_->add_event(acceptchannel_,0);
    started_ = true;
}

void Server::handThisConn() 
{ 
    //baseloop_->update_event(acceptchannel_);
}

void Server::handNewConn() 
{
    //cout<<__FILE__<<"  "<<__LINE__<<endl;
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(listen_fd_, (struct sockaddr *)&client_addr,&client_addr_len)) > 0) 
    {
        
        EventLoop *loop = threadpool_->getNextLoop();
        LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
            << ntohs(client_addr.sin_port);
        // TCP的保活机制默认是关闭的,，可以通过下面几条语句开启
        /*
        int optval = 1;
        socklen_t len_optval = 4;
        setsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        */
        // 限制服务器的最大并发连接数
        if (accept_fd >= MAXFDS) {
            close(accept_fd);
            continue;
        }
        // 设为非阻塞模式
        if (setSocketNonBlocking(accept_fd) < 0) {
            LOG << "Set non block failed!";
            perror("Set non block failed!");
            return;
        }
        //关闭Nagle算法
        setSocketNodelay(accept_fd);
        shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));
    }
}