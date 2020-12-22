/*
 * @Autor: taobo
 * @Date: 2020-05-29 20:08:38
 * @LastEditTime: 2020-06-01 16:14:24
 */
#include "Package.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;

const int BUFFSIZE = 4096;
void shutDownWR(int fd) { shutdown(fd, SHUT_WR); }

ssize_t readn(int fd, void *buff, size_t n) {  //读至某给定的缓冲区
  size_t nleft = n;
  ssize_t nread = 0;
  ssize_t readSum = 0;
  char *ptr = (char *)buff;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else if (errno == EAGAIN) {
        return readSum;
      } else {
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

ssize_t readn(int fd, std::string &inBuffer, bool &zero) {
  ssize_t nread = 0;
  ssize_t readSum = 0;
  while (true) {
    char buf[BUFFSIZE];
    nread = 0;
    if ((nread = read(fd, buf, BUFFSIZE)) < 0) {
      if (errno == EINTR)  // EINTR error
        continue;
      else if (errno == EAGAIN)  // EAGAIN error
      {
        return readSum;
      } else  // real error
      {
        return -1;
      }
    } else if (nread == 0)  // end
    {
      zero = true;
      break;
    }
    readSum += nread;
    inBuffer += string(buf, nread);
  }
  return readSum;
}

ssize_t readn(int fd, std::string &inBuffer) {
  bool zero = false;
  return readn(fd, inBuffer, zero);
}

ssize_t writen(int fd, void *buff, size_t n) {
  ssize_t nleft = n;
  ssize_t nwrite = 0;
  ssize_t nwsum = 0;
  char *ptr = static_cast<char *>(buff);
  while (nleft > 0) {
    if ((nwrite = write(fd, ptr, nleft)) <= 0) {
      if (nwrite < 0) {
        if (errno == EINTR) {
          nwrite = 0;
          continue;
        } else if (errno == EAGAIN) {
          return nwsum;
        } else {
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
ssize_t writen(int fd, std::string &sbuff) {
  size_t nleft = sbuff.size();
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  const char *ptr = sbuff.c_str();
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
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
  if (writeSum == static_cast<int>(sbuff.size()))
    sbuff.clear();
  else
    sbuff = sbuff.substr(writeSum);
  return writeSum;
}

int socket_bind_listen(int port) {
  // socket()
  if (port < 0 || port > 65535) return -1;
  int listen_fd = 0;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
  //为socket设置reuseaddr属性，防止time_wait时地址复用情况
  int optval = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
      0) {
    close(listen_fd);
    return -1;
  }
  // cout<<__FILE__<<"  "<<__LINE__<<"  "<<listen_fd<<" "<<port<<endl;
  // bind()
  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    close(listen_fd);
    return -1;
  }
  // cout<<__FILE__<<"  "<<__LINE__<<"  "<<listen_fd<<" "<<port<<endl;
  // listen()
  if (listen(listen_fd, 2048) < 0)  // SYN queue : 2048
  {
    close(listen_fd);
    return -1;
  }
  // cout<<__FILE__<<"  "<<__LINE__<<"  "<<listen_fd<<" "<<port<<endl;
  return listen_fd;
}

int setSocketNonBlocking(int fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  // cout<<flag<<__FILE__<<"  "<<__LINE__<<endl;
  if (flag < 0) return -1;
  flag |= O_NONBLOCK;
  // cout<<__FILE__<<"  "<<__LINE__<<endl;
  if (fcntl(fd, F_SETFL, flag) < 0) return -1;
  return 0;
}

void handle_for_sigpipe() {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if (sigaction(SIGPIPE, &sa, NULL)) return;
}

void setSocketNodelay(int fd) {
  int enable = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}