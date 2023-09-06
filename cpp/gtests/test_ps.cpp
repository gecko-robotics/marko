#include <gtest/gtest.h>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

using namespace std;

constexpr int port = 8888;
constexpr int LOOP = 5;
string psudpbind = "udp://*:" + to_string(port);
string psudpaddr = "udp://127.0.0.1:"+to_string(port);
string psunix = "unix://./unix.ps.uds";
string psunix2 = "unix://./unix.ps2.uds";

struct psdata_t { int a; };

// data_t ps_test_data[LOOP]{{1},{2},{3},{4},{5}};
vector<psdata_t> ps_test_data;

void callback(const message_t& m) {
  static int i = 0;
  psdata_t d = unpack<psdata_t>(m);
  EXPECT_EQ(d.a, ps_test_data[i++].a);
}

void sub_thread() {
  SubscriberUDP s(sizeof(psdata_t));
  inetaddr_t addr = inet_sockaddr(psudpaddr);
  s.bind(addr);

  // SocketInfo si(s.getSocketFD());
  // si.info("Bind", SocketInfo::UDP);

  s.register_cb( callback );
  for (int i=0; i < LOOP; ++i) {
    s.once();
  }
}

void pub_thread() {
  inetaddr_t addr = inet_sockaddr(psudpaddr);
  PublisherUDP p;
  p.register_addr(addr);
  for (int i=0; i < LOOP; ++i) {
    message_t m = pack<psdata_t>(ps_test_data[i]);
    p.publish(m);
    marko::msleep(1);
  }
}

TEST(marko, pub_sub_udp) {
  for (int i=0; i<LOOP; ++i) ps_test_data.push_back({i});
  thread subth(sub_thread);
  marko::msleep(1);
  thread pubth(pub_thread);

  subth.join();
  pubth.join();
}

/////////////////////////////////////////

void sub_thread_un() {
  unixaddr_t addr = unix_sockaddr(psunix);
  SubscriberUnix s(sizeof(psdata_t));
  s.bind(addr);

  s.register_cb( callback );
  for (int i=0; i < LOOP; ++i) {
    s.once();
  }
}

void pub_thread_un() {
  unixaddr_t addr = unix_sockaddr(psunix);
  PublisherUnix p;
  p.register_addr(addr);
  for (int i=0; i < LOOP; ++i) {
    message_t m = pack<psdata_t>(ps_test_data[i]);
    p.publish(m);
    marko::msleep(1);
  }
}

TEST(marko, pub_sub_unix) {
  for (int i=0; i<LOOP; ++i) ps_test_data.push_back({i});
  thread subth(sub_thread_un);
  marko::msleep(1);
  thread pubth(pub_thread_un);

  subth.join();
  pubth.join();
}
