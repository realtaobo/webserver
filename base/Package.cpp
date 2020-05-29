/*
 * @Autor: taobo
 * @Date: 2020-05-29 20:08:38
 * @LastEditTime: 2020-05-29 21:33:38
 */ 
#include "Package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

const int BUFFSIZE = 4096;
void shutDownWR(int fd) 
{
    shutdown(fd, SHUT_WR);
}

ssize_t readn(int fd, void *buff, size_t n) {//读至某给定的缓冲区
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char *ptr = (char *)buff;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN) 
            {
                return readSum;
            } else 
            {
                return -1;
            }
        } else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer, bool &zero)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true) {
        char buf[BUFFSIZE];
        nread = 0;
        if ((nread = read(fd, buf, BUFFSIZE)) < 0) {
            if(errno == EINTR)//EINTR error
                continue;
            else if (errno == EAGAIN) //EAGAIN error
            {
                return readSum;
            } else //real error
            {
                return -1;
            }
        } else if (nread == 0)//end
        {
            zero = true;
            break;
        }
        readSum += nread;
        inBuffer += string(buf,nread);
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer)
{
    bool zero = false;
    return readn(fd,inBuffer,zero);
}

ssize_t writen(int fd, void *buff, size_t n)
{
    ssize_t nleft = n;
    ssize_t nwrite = 0;
    ssize_t nwsum = 0;
    char* ptr = static_cast<char*>(buff);
    while (nleft > 0)
    {
        if((nwrite = write(fd,ptr,nleft)) <= 0)
        {
            if(nwrite < 0)
            {
                if(errno == EINTR){
                    nwrite = 0;
                    continue;
                }else if(errno == EAGAIN){
                    return nwsum;
                }else{
                    return -1;
                }
            }
        }
        nleft -= nwrite;
        nwsum += nwrite;
        ptr += nwrite;
    }
    return nwsum;
}
ssize_t writen(int fd, std::string &sbuff) 
{
    size_t nleft = sbuff.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char *ptr = sbuff.c_str();
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if(nwritten < 0) {
                if(errno == EINTR) {
                    nwritten = 0;
                    continue;
                } else if (errno == EAGAIN)
                    break;
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    if(writeSum == static_cast<int>(sbuff.size()))
        sbuff.clear();
    else
        sbuff = sbuff.substr(writeSum);
    return writeSum;
}