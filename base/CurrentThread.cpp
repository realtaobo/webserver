/*
 * @Autor: taobo
 * @Date: 2020-05-31 23:36:54
 * @LastEditTime: 2020-12-22 12:42:07
 */

#include <sys/syscall.h>
#include <unistd.h>

pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }
