/*
 * @Autor: taobo
 * @Date: 2020-05-30 19:37:07
 * @LastEditTime: 2020-06-01 19:13:49
 */ 
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
#include "HttpData.h"
#include "../Log/Logger.h"

using namespace std;

HttpData::HttpData(EventLoop* p, int fd):
tcp_server(new tcp_connection(p,fd)),
method_(METHOD_GET),
httpversion_(HTTP_11),
readpos_(0),
state_(STATE_PARSE_URI),
hState_(H_START),
keepalive_(false)
{
    tcp_server->setcall(std::bind(&HttpData::process,this));
    tcp_server->setseper(std::bind(&HttpData::seperateTimer,this));
}

EventLoop* HttpData::getLoop()
{
    return tcp_server->getLoop();
}

void HttpData::handleClose()
{
    tcp_server->handleClose();
}

void HttpData::newEvent()
{
    tcp_server->set_event(EPOLLIN|EPOLLET|EPOLLONESHOT);
    //tcp_server->set_event(EPOLLIN|EPOLLET);
    tcp_server->reg_event();
}
shared_ptr<Channel> HttpData::getChannel()
{
     return tcp_server->getChannel();
}

void HttpData::seperateTimer() {
    if (timer_.lock()) {
        shared_ptr<Timer> my_timer(timer_.lock());
        my_timer->clearreq();
        timer_.reset();
    }
}

void HttpData::reset()
{
    filename_.clear();
    path_.clear();
    readpos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
    keepalive_ = false;
    this->seperateTimer();
}

URIState HttpData::parse_URI()
{
    string& str = tcp_server->getinbuffer();
    // '\r\n' status line
    size_t pos = str.find('\r',readpos_);
    if(pos == string::npos)
    {
        return PARSE_URI_ERROR;
    }
    string request_line  = str.substr(0,pos);//complete request line
    if(str.size() > pos+1)
        str = str.substr(pos+1);
    else
        str.clear();
    int posget = request_line.find("GET");
    int pospost = request_line.find("POST");
    int poshead = request_line.find("HEAD");

    if(posget >= 0){    //get method
        pos = posget;
        method_ = METHOD_GET;
    }else if(pospost >= 0 ){
        pos = pospost;
        method_ = METHOD_POST;
    }else if(poshead >= 0){
        pos = poshead;
        method_ = METHOD_HEAD;
    }else{
        return PARSE_URI_ERROR;
    }

    pos = request_line.find("/",pos);//get uri
    if(pos ==  string::npos){
        filename_ = "index.html";
        httpversion_ = HTTP_11;
        return PARSE_URI_ERROR;
    }else{
        size_t _pos = request_line.find(' ',pos);
        if(_pos == string::npos){
            return PARSE_URI_ERROR;
        }else{
            if(pos+1 == _pos){
                filename_ = "index.html";
            }else{
                filename_ = request_line.substr(pos+1,_pos-pos-1);
                size_t args  = filename_.find('?');
                if(args != string::npos){
                    filename_ = filename_.substr(0,args);
                }
            }
        }
        pos = _pos;
    }

    pos = request_line.find('/',pos);// get http version
    if(pos == string::npos){
        return PARSE_URI_ERROR;
    }else{
        if(request_line.size()-pos < 4){
            return PARSE_URI_ERROR;
        }else{
            string tmp = request_line.substr(pos+1,3);
            if(tmp == "1.0")
                httpversion_ = HTTP_10;
            else if(tmp == "1.1")
                httpversion_ = HTTP_11;
            else 
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parse_Headers()
{
    string& str = tcp_server->getinbuffer();
    int keys = -1, keye = -1, vals = -1, vale = -1;
    int nowpos_ = 0;
    bool notfinsh = true;
    size_t i = 0;
    for(;i < str.size() && notfinsh; ++i)
    {
        switch(hState_)
        {
            case H_START:{
                if(str[i] == '\n' || str[i] == '\r') break;
                hState_ = H_KEY;
                keys = i;
                nowpos_ = i;
                break;
            }
            case H_KEY:{
                if(str[i] == ':'){
                    keye = i;
                    if(keye <= keys) return PARSE_HEADER_ERROR;
                    hState_ = H_COLON;
                }else if(str[i] == '\n' || str[i] == '\r'){
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_COLON:{
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
            case H_VALUE:{
                if(str[i] == '\r'){
                    hState_ = H_CR;
                    vale = i;
                    if(vale <= vals) return PARSE_HEADER_ERROR;
                }else if(i - vals > 255){
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_CR:{
                if(str[i] == '\n'){
                    hState_ = H_LF;
                    string key(str.begin()+keys,str.begin()+keye);
                    string value(str.begin()+vals,str.begin()+vale);
                    headers_[key] = value;
                    nowpos_ = i;
                }else{
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_LF:{
                if(str[i] == '\r'){
                    hState_ = H_END_CR;
                }else{
                    keys = i;
                    hState_ = H_KEY;
                }
                break;
            }
            case H_END_CR:{
                if(str[i] == '\n'){
                    hState_ = H_END_LF;
                }else{
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_END_LF:{
                notfinsh = false;
                nowpos_ = i;
                break;
            }
        }//end switch
    }//end for
    if(hState_ == H_END_LF){
        str = str.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(nowpos_);
    return PARSE_HEADER_AGAIN;
}

AnalysisState HttpData::analysisRequest()
{
    if(method_ == METHOD_POST){}
    else if(method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        if(!filename_.empty())
        {
            string out("HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World");
            tcp_server->setoutbuffer(move(out));
            return ANALYSIS_SUCCESS;
        }
    }
    return ANALYSIS_ERROR;
}

int HttpData::process()
{
    if(state_ == STATE_PARSE_URI)
    {
        URIState flag = parse_URI();
        if(flag == PARSE_URI_AGAIN)
            return -1;
        else if(flag == PARSE_URI_ERROR){
            LOG <<"process uri error..." ;
            tcp_server->clearanderror();
            return -2;
        }else{
            state_ = STATE_PARSE_HEADERS;
        }       
    }
    if(state_ == STATE_PARSE_HEADERS)
    {
        HeaderState flag = parse_Headers();
        if(flag == PARSE_HEADER_AGAIN){
            return -1;
        }else if(flag == PARSE_HEADER_ERROR){
            LOG<<"process header error..." ;
            tcp_server->clearanderror();
            return -2;
        }
        if(method_ == METHOD_POST){
            state_ = STATE_RECV_BODY;
        }else{
            state_ = STATE_ANALYSIS;
        }
    }
    string& buf = tcp_server->getinbuffer();
    if(state_ == STATE_RECV_BODY){
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