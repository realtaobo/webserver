/*
 * @Autor: taobo
 * @Date: 2020-05-26 20:41:40
 * @LastEditTime: 2020-05-27 21:11:49
 * @Description: file content
 */ 

#include <iostream>
#include <thread>
#include <chrono>

#include "thread.h"
#include "../Log/AsyncLogging.h"

using namespace std;

void func1()
{
    this_thread::sleep_for(std::chrono::seconds(1));
    cout<<"the thread start...."<<std::this_thread::get_id()<<endl;
    
}
void func2()
{
    this_thread::sleep_for(std::chrono::seconds(4));
    cout<<"the thread start...."<<std::this_thread::get_id()<<endl;
    
}

// int main()
// {
//     Thread p1(func1,"taobo");
//     Thread p2(func2);
//     p1.start();
//     p2.start();
//     cout<<p1.tid_<<" "<<p2.tid_<<endl;
//     cout<<p1.name_<<" "<<p2.name_<<endl;
//     while(1);
//     return 0;
// }

















            // "args": [
            //     "-g",
            //     "-Wall",
            //     "${file}",
            //     "-o",
            //     "${fileDirname}/${fileBasenameNoExtension}",
            //     "-std=c++11",
            //     "-pthread"
            // ],