/*
 * @Autor: taobo
 * @Date: 2020-05-29 10:45:30
 * @LastEditTime: 2020-05-31 11:06:47
 * @Description: file content
 */ 
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Tcp_Connection.h"
#include "EventLoop.h"
#include "Timer.h"

using namespace std;

enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };
enum HttpVersion { HTTP_10 = 1, HTTP_11 };

enum ProcessState {
  STATE_PARSE_URI = 1,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_FINISH
};

enum URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
  PARSE_URI_SUCCESS,
};

enum HeaderState {
  PARSE_HEADER_SUCCESS = 1,
  PARSE_HEADER_AGAIN,
  PARSE_HEADER_ERROR
};

enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

enum ParseState {
  H_START = 0,
  H_KEY,
  H_COLON,
  H_SPACES_AFTER_COLON,
  H_VALUE,
  H_CR,
  H_LF,
  H_END_CR,
  H_END_LF
};

class HttpData: public std::enable_shared_from_this<HttpData>
{
private:
    shared_ptr<tcp_connection>  tcp_server;
    HttpMethod method_;//状态行
    HttpVersion httpversion_;

    string filename_;
    string path_;
    int readpos_;
    //消息处理相关标记
    ProcessState state_;
    ParseState hState_;

    bool keepalive_;
    map<string,string> headers_;
    weak_ptr<Timer> timer_;
public:
    //下面几个为普通成员构造函数
    HttpData(EventLoop* p, int fd);
    ~HttpData() = default;  //要关闭fd才可以
    void reset();
    void seperateTimer();
    EventLoop *getLoop();
    void handleClose();
    void newEvent();
public:
    //提供一个钩子，当添加当前连接的FD到epoll队列上时，由linkTimer()其初始化timer_
    void linkTimer(shared_ptr<Timer> m) { timer_ = m; }
    
};