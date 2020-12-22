/*
 * @Autor: taobo
 * @Date: 2020-05-29 20:04:37
 * @LastEditTime: 2020-12-22 12:41:10
 * @Description: 非阻塞IO必备的一些包裹函数，与基于C/S架构的部分函数的包裹函数
 */
#pragma once
#include <stdlib.h>

#include <string>

void shutDownWR(int fd);

/*连接建立，若某一端关闭连接，而另一端仍然向它写数据，
 *第一次写数据后会收到RST响应，此后再写数据，内核将向进程发出SIGPIPE信号，通知进程此连接已经断开。
 *而SIGPIPE信号的默认处理是终止程序，为避免这种情况发生需要忽略此信号，这也是下面handle_for_sigpipe函数的作用
 */
void handle_for_sigpipe();  //处理sigpipe信号

int socket_bind_listen(int port);  // socket(),bind(),listen()

int setSocketNonBlocking(int fd);

// TCP_NODELAY是针对tcp套接字专用的选项，目的是为了禁止nagle算法。
// nagle算法为了防止网络传输中的小分组太多，影响网络性能，会自动将收到的多个数据包的返回包，整合成一个比较大的分组返回给对方，
// 这种情况下，请求方在发送前几个小分组的时候可能不能立即收到对方的响应，
// 在某些实时性要求比较高的应用中，需要屏蔽掉nagle算法，好让请求离开得到响应。
// 下面的setSocketNodelay函数就是设置TCP_NODELAY属性，以关闭Nagle算法的
void setSocketNodelay(int fd);

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
