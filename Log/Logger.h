/*
 * @Autor: taobo
 * @Date: 2020-05-27 23:53:28
 * @LastEditTime: 2020-05-28 11:46:02
 * @Description: 创建一个日志文件的对外接口，此处没有似单例模式一样创建一个单例类，而是类似
 * 调用pthread_once创建了一个全局唯一的静态对象，在C++11中可以使用 call_once替代pthread_once
 */ 
#pragma once
#include <cstring>
#include <string>
#include "LogBuffer.h"
#include "LogStream.h"

using namespace std;

class Logger
{
private:
    class Impl
    {
    public:
        Impl(const string& fileName, int line);
        void formatTime();//在日志消息上加上时间头
        LogStream stream_;
        int line_;
        string basename_;
    };
    Impl impl_;
    static string logFileName_;
public:
    //fileName表示日志产生源文件， line表示行号
    Logger(const string& fileName, int line);
    ~Logger();
    LogStream& stream();
    static void setLogFileName(const string& Logfilename);
    static const string& getLogFileName();

};

#define LOG Logger(__FILE__,__LINE__).stream()
//__FILE__与__LINE__均是编译器内置宏
//__LINE__：在源代码中插入当前源代码行号；
//__FILE__：在源文件中插入当前源文件名；