/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:01:02
 * @LastEditTime: 2020-05-26 13:01:14
 */ 
#include <iostream>
#include <getopt.h>
#include <string>


int main(int argc, char* argv[])
{
    int threadNum = 4;
    int port = 826;
    std::string logPath = "./webserver.log";
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
                std::cout<<"logPath should start with \\"<<'\n';
                std::abort();
            }
        }else if(opt=='p')  //port setting
        {
            port = std::atoi(optarg);
        }
    }
    //std::cout<<threadNum<<" "<<port<<" "<<logPath<<"\n";
    
    return 0;
}
