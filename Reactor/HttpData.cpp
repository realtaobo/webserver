/*
 * @Autor: taobo
 * @Date: 2020-05-30 19:37:07
 * @LastEditTime: 2020-06-01 23:01:21
 */
#include "HttpData.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "../Log/Logger.h"
#include "EventLoop.h"
#include "Tcp_Connection.h"
#include "Timer.h"

using namespace std;

char favicon[555] = {
    '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};

HttpData::HttpData(EventLoop* p, int fd)
    : tcp_server(new tcp_connection(p, fd)),
      method_(METHOD_GET),
      httpversion_(HTTP_11),
      readpos_(0),
      state_(STATE_PARSE_URI),
      hState_(H_START),
      keepalive_(false) {
  tcp_server->setcall(std::bind(&HttpData::process, this));
  tcp_server->setseper(std::bind(&HttpData::seperateTimer, this));
}

EventLoop* HttpData::getLoop() { return tcp_server->getLoop(); }

void HttpData::handleClose() { tcp_server->handleClose(); }

void HttpData::newEvent() {
  tcp_server->set_event(EPOLLIN | EPOLLET | EPOLLONESHOT);
  // tcp_server->set_event(EPOLLIN|EPOLLET);
  tcp_server->reg_event();
}
shared_ptr<Channel> HttpData::getChannel() { return tcp_server->getChannel(); }

void HttpData::seperateTimer() {
  if (timer_.lock()) {
    shared_ptr<Timer> my_timer(timer_.lock());
    my_timer->clearreq();
    timer_.reset();
  }
}

void HttpData::reset() {
  filename_.clear();
  path_.clear();
  readpos_ = 0;
  state_ = STATE_PARSE_URI;
  hState_ = H_START;
  headers_.clear();
  keepalive_ = false;
  // this->seperateTimer();
}

URIState HttpData::parse_URI() {
  string& str = tcp_server->getinbuffer();
  // '\r\n' status line
  size_t pos = str.find('\r', readpos_);
  if (pos == string::npos) {
    return PARSE_URI_ERROR;
  }
  string request_line = str.substr(0, pos);  // complete request line
  if (str.size() > pos + 1)
    str = str.substr(pos + 1);
  else
    str.clear();
  int posget = request_line.find("GET");
  int pospost = request_line.find("POST");
  int poshead = request_line.find("HEAD");

  if (posget >= 0) {  // get method
    pos = posget;
    method_ = METHOD_GET;
  } else if (pospost >= 0) {
    pos = pospost;
    method_ = METHOD_POST;
  } else if (poshead >= 0) {
    pos = poshead;
    method_ = METHOD_HEAD;
  } else {
    return PARSE_URI_ERROR;
  }

  pos = request_line.find("/", pos);  // get uri
  if (pos == string::npos) {
    filename_ = "index.html";
    httpversion_ = HTTP_11;
    return PARSE_URI_ERROR;
  } else {
    size_t _pos = request_line.find(' ', pos);
    if (_pos == string::npos) {
      return PARSE_URI_ERROR;
    } else {
      if (pos + 1 == _pos) {
        filename_ = "index.html";
      } else {
        filename_ = request_line.substr(pos + 1, _pos - pos - 1);
        size_t args = filename_.find('?');
        if (args != string::npos) {
          filename_ = filename_.substr(0, args);
        }
      }
    }
    pos = _pos;
  }

  pos = request_line.find('/', pos);  // get http version
  if (pos == string::npos) {
    return PARSE_URI_ERROR;
  } else {
    if (request_line.size() - pos < 4) {
      return PARSE_URI_ERROR;
    } else {
      string tmp = request_line.substr(pos + 1, 3);
      if (tmp == "1.0")
        httpversion_ = HTTP_10;
      else if (tmp == "1.1")
        httpversion_ = HTTP_11;
      else
        return PARSE_URI_ERROR;
    }
  }
  return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parse_Headers() {
  string& str = tcp_server->getinbuffer();
  int keys = -1, keye = -1, vals = -1, vale = -1;
  int nowpos_ = 0;
  bool notfinsh = true;
  size_t i = 0;
  for (; i < str.size() && notfinsh; ++i) {
    switch (hState_) {
      case H_START: {
        if (str[i] == '\n' || str[i] == '\r') break;
        hState_ = H_KEY;
        keys = i;
        nowpos_ = i;
        break;
      }
      case H_KEY: {
        if (str[i] == ':') {
          keye = i;
          if (keye <= keys) return PARSE_HEADER_ERROR;
          hState_ = H_COLON;
        } else if (str[i] == '\n' || str[i] == '\r') {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_COLON: {
        if (str[i] == ' ') {
          hState_ = H_SPACES_AFTER_COLON;
        } else
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_SPACES_AFTER_COLON: {
        hState_ = H_VALUE;
        vals = i;
        break;
      }
      case H_VALUE: {
        if (str[i] == '\r') {
          hState_ = H_CR;
          vale = i;
          if (vale <= vals) return PARSE_HEADER_ERROR;
        } else if (i - vals > 255) {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_CR: {
        if (str[i] == '\n') {
          hState_ = H_LF;
          string key(str.begin() + keys, str.begin() + keye);
          string value(str.begin() + vals, str.begin() + vale);
          headers_[key] = value;
          nowpos_ = i;
        } else {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_LF: {
        if (str[i] == '\r') {
          hState_ = H_END_CR;
        } else {
          keys = i;
          hState_ = H_KEY;
        }
        break;
      }
      case H_END_CR: {
        if (str[i] == '\n') {
          hState_ = H_END_LF;
        } else {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_END_LF: {
        notfinsh = false;
        nowpos_ = i;
        break;
      }
    }  // end switch
  }    // end for
  if (hState_ == H_END_LF) {
    str = str.substr(i);
    return PARSE_HEADER_SUCCESS;
  }
  str = str.substr(nowpos_);
  return PARSE_HEADER_AGAIN;
}

AnalysisState HttpData::analysisRequest() {
  cout << filename_ << " " << method_ << endl;
  if (method_ == METHOD_POST) {
  } else if (method_ == METHOD_GET || method_ == METHOD_HEAD) {
    if (filename_ == "index.html") {
      string out(
          "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World");
      tcp_server->setoutbuffer(move(out));
      return ANALYSIS_SUCCESS;
    } else if (filename_ == "favicon.ico") {
      string header("HTTP/1.1 200 OK\r\n");
      header += "Content-Type: image/png\r\n";
      header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
      header += "Server: LinYa's Web Server\r\n";
      header += "\r\n";
      header += string(favicon, favicon + sizeof favicon);
      tcp_server->setoutbuffer(move(header));
      return ANALYSIS_SUCCESS;
    } else if (!filename_.empty()) {  //其他字符返回小笔记
      string body_buff;
      body_buff += "<html><title>Notes</title>";
      body_buff += "<h1>我的博客</h1>";
      body_buff +=
          "<meta http-equiv=\"Content-Type\" content=\"text/html; "
          "charset=UTF-8\">";

      body_buff +=
          "<dt><a "
          "href=\"https://gitee.com/windyGOlife/webserver/\">本项目仓库——码云</"
          "a></dt>";
      body_buff +=
          "<dt><a href=\"https://gitee.com/windyGOlife/cmianshi/\">其他个人 "
          "笔记</a></dt>";
      body_buff += "<dt><a href=\"http://www.linya.pub/\">C++ 笔记</a></dt>";
      body_buff +=
          "<dt><a "
          "href=\"https://blog.csdn.net/qq_41345173/article/details/"
          "105491017\">三次握手中的性能优化与安全问题</a></dt>";
      //...... 若有兴趣，可以据此继续添加
      body_buff += "</html>";
      string header("HTTP/1.1 200 OK\r\n");
      // header += "Content-Type: text/html\r\n";
      header += "Connection: Close\r\n";
      header += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
      header += "\r\n";
      tcp_server->setoutbuffer(header + body_buff);
      return ANALYSIS_SUCCESS;
    }
  }
  return ANALYSIS_ERROR;
}

int HttpData::process() {
  if (state_ == STATE_PARSE_URI) {
    URIState flag = parse_URI();
    if (flag == PARSE_URI_AGAIN)
      return -1;
    else if (flag == PARSE_URI_ERROR) {
      LOG << "process uri error...";
      tcp_server->clearanderror();
      return -2;
    } else {
      state_ = STATE_PARSE_HEADERS;
    }
  }
  if (state_ == STATE_PARSE_HEADERS) {
    HeaderState flag = parse_Headers();
    if (flag == PARSE_HEADER_AGAIN) {
      return -1;
    } else if (flag == PARSE_HEADER_ERROR) {
      LOG << "process header error...";
      tcp_server->clearanderror();
      return -2;
    }
    if (method_ == METHOD_POST) {
      state_ = STATE_RECV_BODY;
    } else {
      state_ = STATE_ANALYSIS;
    }
  }
  string& buf = tcp_server->getinbuffer();
  if (state_ == STATE_RECV_BODY) {
    int content_length = -1;
    if (headers_.find("Content-length") != headers_.end()) {
      content_length = stoi(headers_["Content-length"]);
    } else {
      // cout << "(state_ == STATE_RECV_BODY)" << endl;
      tcp_server->clearanderror();
      return -2;
    }
    if (static_cast<int>(buf.size()) < content_length) return -2;
    state_ = STATE_ANALYSIS;
  }
  if (state_ == STATE_ANALYSIS) {
    AnalysisState flag = analysisRequest();
    if (flag == ANALYSIS_SUCCESS) {
      state_ = STATE_FINISH;
      reset();
      return 0;
    } else {
      // cout << "state_ == STATE_ANALYSIS" << endl;
      tcp_server->clearanderror();
      return -2;
    }
  }
  return -2;
}
