/*
 * @Autor: taobo
 * @Date: 2020-05-26 14:29:11
 * @LastEditTime: 2020-05-27 21:16:21
 */ 
#include "LogFile.h"
#include <mutex>

using namespace std;

LogFile::LogFile(const string& basename, int flushlen):
    filename_(basename),
    flushEveryN_(flushlen),
    cnt_(0),
    mtx_(new mutex),
    file_(new AppendFile(basename))
    {}

void LogFile::append(const char* logline, size_t len)
{
    unique_lock<mutex> lck(*(this->mtx_));
    this->append_unlocked(logline,len);
}

void LogFile::append_unlocked(const char* logline, size_t len)
{
    this->file_->append(logline,len);
    ++cnt_;
    if(cnt_>=flushEveryN_)
    {
        cnt_ = 0;
        this->file_->flush();
    }
}

void LogFile::flush()
{
    unique_lock<mutex> lck(*(this->mtx_));
    this->file_->flush();
}