/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:01:02
 * @LastEditTime: 2020-06-01 20:11:54
 */ 
#include <iostream>
#include <getopt.h>
#include <string>

#include  "Log/Logger.h"
#include  "Reactor/EventLoop.h"
#include  "Reactor/Server.h"

using namespace std;

int main(int argc, char* argv[])
{
    int threadNum = 4;
    int port = 826;
    string logPath = "./webserver.log";
    //处理参数
    int opt;
    const char *str = "t:l:p:";
    while ((opt = getopt(argc,argv,str)) != -1)
    {
        if(opt=='t')    //thread nums
        {
            threadNum = std::atoi(optarg);
        }else if(opt=='l')// log file dest
        {
            logPath = optarg;
            if(logPath.size()<2 || optarg[0]!='/' )
            {
                cout<<"logPath should start with /"<<'\n';
                std::abort();
            }
        }else if(opt=='p')  //port setting
        {
            port = std::atoi(optarg);
        }
    }
    cout<<threadNum<<" "<<port<<" "<<logPath<<"\n";
    Logger::setLogFileName(logPath);
    
    EventLoop mainloop;
    Server myserver(&mainloop,port,threadNum);
    cout<<__FILE__<<"  "<<__LINE__<<endl;
    myserver.start();
    mainloop.loop();
    return 0;
}
