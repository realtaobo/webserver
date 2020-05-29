<!--
 * @Autor: taobo
 * @Date: 2020-05-28 15:14:39
 * @LastEditTime: 2020-05-29 23:00:08
 * @Description: file content
--> 
# Log设计
仿照了[muduo库](https://github.com/chenshuo/muduo)的设计,但发现实现过于复杂，使用了大量C风格的字符串，维护过于麻烦，重新基于`std::string`进行了简单的缓冲区设计  
#### Log相关的类包括
<font color=red>

类|定义文件|类|定义文件
--|--|--|---
AppendFile |(FileUtil.h)|LogFile| (LogFile.h)
AsyncLogging| (AsyncLogging.h)|FixedBuffer| (LogBuffer.h)
LogStream |(LogStream.h)|Logger| (Logger.h)

</font>

#### Log类的简单实现
```cpp
class AppendFile{
    FILE* fp_;
    char buffer_[64*1024];  //64KB
```
```cpp
class LogFile{
    string filename_;
    const int flushEveryN_;
    int cnt_;//当cnt==flushEveryN_时将内存日志刷到磁盘上
    unique_ptr<mutex> mtx_; //保护日志锁
    unique_ptr<AppendFile> file_;//底层类对象
```
```cpp
template<int Size>
class FixedBuffer{
    vector<string> data_;
    size_t used_ ;
    size_t size_;
```
```cpp
class AsyncLogging: noncopyable
{
    typedef FixedBuffer<b_BufferSize> Buffer;
    typedef shared_ptr<Buffer> BufferPtr;
    typedef vector<BufferPtr> BufferVector;
    string basename_;
    Thread thread_; 
    BufferPtr currentBuffer_;//前端缓冲区1
    BufferPtr nextBuffer_;  //前端缓冲区2，当1满了后相互调换
    BufferVector buffers_;  //后端，负责向磁盘写
```
```cpp
class Logger
{
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
```
#### Log类从属关系
AppendFile , LogFile , AsyncLogging , Logger四个类是构成日志子模块的核心，其间关系层层递进，由底层到对外接口。其中每一个类都有对应的append函数负责完成日志写入。AsyncLogging类负责维护与开启日志线程，在类内定义了对应的缓冲区(类型为FixedBuffer)，故该线程采取生产者——消费者模型，基于信号变量与临界区实现主要流程。  
其中FileUtil是最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件，底层使用了标准IO，该append函数直接向文件写。LogFile进一步封装了FileUtil，并设置了一个循环次数flushEveryN_，每过这么多次就flush一次。故当启动一个Log线程调度之后若发现AsyncLogging的缓冲区buffers_不为空就调用LogFile.append(),最后写入底层文件。  
Logger的核心是LogStream类，LogStream类主要用来格式化输出，重载了`<<`运算符，同时也有自己的一块缓冲区，这里缓冲区的存在是为了缓存一行，把多个`<<`的结果连成一块。而在具体使用时则基于Logger提供的宏实现。
#### Log类使用
```cpp
#define LOG Logger(__FILE__,__LINE__).stream()
```
上宏将返回一个LogStream对象，可以借助该对象的`<<`运算符实现简单使用，示例如下：
```cpp
LOG << thread446_9 ;

2020-05-28 12:46:09
thread446_9 -- /home/go/webserver/Log/log_test.cpp:34
```
