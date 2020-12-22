/*
 * @Autor: taobo
 * @Date: 2020-05-26 13:01:25
 * @LastEditTime: 2020-05-26 20:54:27
 */
#pragma once

class noncopyable {
 protected:
  noncopyable() = default;
  ~noncopyable() = default;

 private:
  noncopyable(const noncopyable&) = delete;
  // noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};
