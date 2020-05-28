/*
 * @Autor: taobo
 * @Date: 2020-05-28 10:42:39
 * @LastEditTime: 2020-05-28 12:41:46
 * @Description: Logger 的实现
 */ 
#include <cstring>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <mutex>
#include "LogBuffer.h"
#include "LogStream.h"
#include "AsyncLogging.h"
#include "Logger.h"

using namespace std;

string Logger::logFileName_ = "./WebServer.log";

void Logger::setLogFileName(const string& Logfilename)
{
    logFileName_ = Logfilename;
}

const string& Logger::getLogFileName()
{
    return logFileName_;
}

Logger::Impl::Impl(const string& fileName, int line)
: line_(line),basename_(fileName) {
    formatTime();
}

void Logger::Impl::formatTime()//muduo
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv,NULL);//https://linux.die.net/man/2/gettimeofday
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);//http://www.cplusplus.com/reference/ctime/localtime/?kw=localtime
    strftime(str_t,26,"%Y-%m-%d %H:%M:%S\n", p_time);//<time.h>
    stream_ << str_t;
}

Logger::Logger(const string& fileName, int line)
: impl_(fileName,line)  {}

LogStream& Logger::stream()
{
    return impl_.stream_ ;
}

static std::once_flag once_control_ ;
static AsyncLogging* AsyncLogger_ ;

void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const string& msg)
{
    std::call_once(once_control_,once_init);
    AsyncLogger_->append(msg);
}

//Logger临时对象析构之时将当前日志信息写入文件
Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    output(impl_.stream_.data());
}