/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:54:49
 * @LastEditTime: 2020-05-27 21:04:31
 * @Description: 最底层的日志文件类AppendFile的实现
 */ 
#include "FileUtil.h"
#include <stdio.h>

using namespace std;


AppendFile::AppendFile(const std::string& filename)
{
    fp_ = fopen(filename.c_str(),"ae");//'e' for O_CLOEXEC
    setbuffer(fp_,buffer_,sizeof buffer_);
}

AppendFile::~AppendFile()
{
    fclose(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return fwrite_unlocked(logline,1,len,fp_);//非阻塞写文件
}

void AppendFile::flush()
{
    fflush(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
    size_t n = this->write(logline,len);
    size_t domain = len-n;
    while (domain>0)
    {
        size_t tmp = this->write(logline+n,domain);
        if(tmp==0)
        {
            int err = ferror(this->fp_);//当发生错误时返回非0值
            if(err)//a error occur
            {
                fprintf(stderr,"AppendFile::append() failed !\n");
            }
            break;
        }
        n += tmp;
        domain -= tmp;
    }
    
}