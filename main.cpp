/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:01:02
 * @LastEditTime: 2020-05-28 23:12:03
 */ 
#include <iostream>
#include <getopt.h>
#include <string>

#include  "Log/Logger.h"

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
    
    return 0;
}