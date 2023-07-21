/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
#pragma once

#include <chrono> // time: sec, msec, usec
#include <thread>

namespace marko {

inline void msleep(int msec) {
  std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}
inline void usleep(int usec) {
  std::this_thread::sleep_for(std::chrono::microseconds(usec));
}
inline void sleep(int sec) {
  std::this_thread::sleep_for(std::chrono::seconds(sec));
}

} // namespace marko

typedef struct timeval timeval_t;

static
timeval_t get_time(const long timeout_msec) {
  // long sec  = 0;
  // long msec = 0;

  // if (timeout_msec >= 1000) {
  //   sec = timeout_msec / 1000;
  //   msec = timeout_msec % 1000;
  // }

  timeval_t tv;
  tv.tv_sec  = long(timeout_msec / 1000);
  tv.tv_usec = (timeout_msec - tv.tv_sec*1000) % 1000;
  return std::move(tv);
} // end marko
