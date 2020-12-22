/*
 * @Autor: taobo
 * @Date: 2020-06-02 09:46:02
 * @LastEditTime: 2020-12-22 13:27:20
 * @Description: file content
 */
#include <getopt.h>

#include <iostream>
#include <string>

#include "Reactor/EventLoop.h"
#include "Reactor/Server.h"
//关闭系统中的日志等输入输出模块，进行压力测试
//不支持Http长连接，可以通过setsockopt打开socket keepalive属性
using namespace std;

int main(int argc, char* argv[]) {
  int threadNum = 4;
  int port = 826;
  //处理参数
  int opt;
  const char* str = "t:p:";
  while ((opt = getopt(argc, argv, str)) != -1) {
    if (opt == 't')  // thread nums
    {
      threadNum = std::atoi(optarg);
    } else if (opt == 'p')  // port setting
    {
      port = std::atoi(optarg);
    }
  }
  EventLoop mainloop;
  Server myserver(&mainloop, port, threadNum);
  myserver.start();
  mainloop.loop();
  return 0;
}
