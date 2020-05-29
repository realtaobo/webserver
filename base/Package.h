/*
 * @Autor: taobo
 * @Date: 2020-05-29 20:04:37
 * @LastEditTime: 2020-05-29 21:34:59
 * @Description: 非阻塞IO必备的一些包裹函数，与基于C/S架构的部分函数的包裹函数
 */ 
#pragma once
#include <stdlib.h>
#include <string>

void shutDownWR(int fd);

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
