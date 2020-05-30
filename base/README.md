<!--
 * @Autor: taobo
 * @Date: 2020-05-28 23:19:35
 * @LastEditTime: 2020-05-30 18:25:23
 * @Description: 简述类
--> 
## base 文件夹下类信息
类名|定义文件|作用
---|----|---
CountDownLatch|CountDownLatch.h|确保线程入口函数执行起来之后主线程在返回
CurrentThread|CurrentThread.h|一个命名空间，保存当前线程的基本信息，创建线程时初始化
noncopyable|noncopyable.h|限制其他类的copy constr函数时使用
Thread|thread.h|对std::thread类的封装 
--|Package.h|  一些常用函数的包裹函数实现