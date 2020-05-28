/*
 * @Autor: taobo
 * @Date: 2020-05-26 15:08:00
 * @LastEditTime: 2020-05-27 21:54:33
 * @Description: log缓冲区设计与实现
 */ 
#pragma once
#include "../base/noncopyable.h"
#include "LogFile.h"
#include "FileUtil.h"

#include <vector>
#include <string>

using namespace std;

const int stream_Size = 1;
const int s_BufferSize = 40;
const int b_BufferSize = 40*10;

template<int Size>
class FixedBuffer : noncopyable
{
private:
    vector<string> data_;
    size_t used_ ;
    size_t size_;
public:
    FixedBuffer():used_(0),size_(Size) {
        data_.resize(size_);
    }
    ~FixedBuffer() = default;
    void append(const string& buf)
    {
        if(used_<size_){
            data_[used_++]=buf;
        }
    }
    bool avail()
    {
        return used_ < size_ ;
    }
    vector<string> data()
    {
        return data_;
    }
    int length(){ return used_;}
    void bzero(){ used_=0;}
    void send_data(LogFile& output)
    {
        for(size_t i=0;i<used_;++i)
        {
            output.append(data_[i].c_str(),data_[i].length());
        }
    }
};