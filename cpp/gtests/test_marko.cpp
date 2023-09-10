// https://google.github.io/googletest/primer.html
#include <gtest/gtest.h>
#include <marko/marko.hpp>
#include <string>
#include <iostream>
#include <stdint.h>

using namespace std;

struct __attribute__((packed)) data_t {
  uint8_t u;
  int a;
  long l;
  float f;
  double d;
};

// data_t test_data[LOOP]{{1},{2},{3},{4},{5}};

TEST(marko, addresses) {
  int port = 9999;
  string host = "1.2.3.4";
  string address = "udp://"+host+":"+to_string(port);
  inetaddr_t addr = inet_sockaddr(address);
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr(host.c_str()));
  EXPECT_EQ(addr.sin_port, htons(port));

  port = 99;
  host = "bc";
  address = "udp://"+host+":"+to_string(port);
  addr = inet_sockaddr(address);
  // cerr << address << " " << host << endl;
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr("255.255.255.255"));
  EXPECT_EQ(addr.sin_port, htons(port));
}

TEST(marko, ascii) {
  Ascii a;
  string orig{"bob|tom|alice|george"};
  // ascii_t msg = a.unpack(orig);
  // EXPECT_TRUE("bob" == msg[0]);
  // EXPECT_TRUE("tom" == msg[1]);
  // EXPECT_TRUE("alice" == msg[2]);
  // EXPECT_TRUE("george" == msg[3]);
  // string redo = a.pack(msg);
  // EXPECT_TRUE(redo == orig);
  // // cerr << "ascii" << endl;
}

TEST(marko, message_t) {
  data_t d{5,-500,10000,3.14176,-3.14147679801};
  message_t m = pack<data_t>(d);
  data_t e = unpack<data_t>(m);
  EXPECT_EQ(d.u, e.u);
  EXPECT_EQ(d.a, e.a);
  EXPECT_EQ(d.l, e.l);
  EXPECT_FLOAT_EQ(d.f, e.f);
  EXPECT_DOUBLE_EQ(d.d, e.d);
  EXPECT_EQ(sizeof(d), sizeof(e));
}