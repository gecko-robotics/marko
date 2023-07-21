#include <gtest/gtest.h>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

using namespace std;

constexpr int port = 8888;
constexpr int LOOP = 5;
string udpbind = "udp://*:" + to_string(port);
string udpaddr = "udp://127.0.0.1:"+to_string(port);
string unix = "unix://./unix.ps.uds";

struct data_t { int a; };

// data_t test_data[LOOP]{{1},{2},{3},{4},{5}};
vector<data_t> test_data;

void callback(const message_t& m) {
  static int i = 0;
  data_t d = unpack<data_t>(m);
  EXPECT_EQ(d.a, test_data[i++].a);
}

void sub_thread() {
  SubscriberUDP s(sizeof(data_t));
  s.bind(udpbind);

  // SocketInfo si(s.getSocketFD());
  // si.info("Bind", SocketInfo::UDP);

  s.register_cb( callback );
  for (int i=0; i < LOOP; ++i) {
    s.once();
  }
}

void pub_thread() {
  inetaddr_t addr = inet_sockaddr(udpaddr);
  PublisherUDP p;
  p.register_addr(addr);
  for (int i=0; i < LOOP; ++i) {
    message_t m = pack<data_t>(test_data[i]);
    p.publish(m);
    marko::msleep(1);
  }
}

TEST(marko, pub_sub_udp) {
  for (int i=0; i<LOOP; ++i) test_data.push_back({i});
  thread subth(sub_thread);
  marko::msleep(1);
  thread pubth(pub_thread);

  subth.join();
  pubth.join();
}

/////////////////////////////////////////

void sub_thread_un() {
  SubscriberUnix s(sizeof(data_t));
  s.bind(unix);

  s.register_cb( callback );
  for (int i=0; i < LOOP; ++i) {
    s.once();
  }
}

void pub_thread_un() {
  unixaddr_t addr = unix_sockaddr(unix);
  PublisherUnix p;
  p.register_addr(addr);
  for (int i=0; i < LOOP; ++i) {
    message_t m = pack<data_t>(test_data[i]);
    p.publish(m);
    marko::msleep(1);
  }
}

TEST(marko, pub_sub_unix) {
  for (int i=0; i<LOOP; ++i) test_data.push_back({i});
  thread subth(sub_thread_un);
  marko::msleep(1);
  thread pubth(pub_thread_un);

  subth.join();
  pubth.join();
}