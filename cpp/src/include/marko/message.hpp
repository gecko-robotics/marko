
#pragma once

#include <vector>
#include <iostream>
#include <ostream>

/*
DEC =    BIN    = HEX
240 = 1111 0000 = F0
170 = 1010 1010 = AA
85  = 0101 0101 = 55
15  = 0000 1111 = 0F
*/

struct __attribute__((packed)) subscription_t {
  uint8_t topic;
  uint8_t status;
};

using message_t = std::vector<uint8_t>;
constexpr uint8_t SUBSCRIBE = 0xAA;
constexpr uint8_t UNSUBSCRIBE = 0x55;

template<typename T>
message_t pack(const T& d) {
  message_t msg(sizeof(d));
  memcpy(static_cast<uint8_t*>(msg.data()), &d, sizeof(d));
  return std::move(msg);
}

template<typename T>
T unpack(const message_t& m) {
  T d;
  memcpy(&d, m.data(), m.size());
  return std::move(d);
}

static
std::string msg2string(const message_t& msg) {
  std::string s;
  if (msg.size() == 0) return s;
  s += "[";
  for (const uint8_t& m: msg) {
    s += std::to_string(int(m));
    s += ",";
  }
  s += "]";
  return s;
}

static
std::ostream &operator<<(std::ostream &os, message_t const &msg) {
  return os << msg2string(msg);
}