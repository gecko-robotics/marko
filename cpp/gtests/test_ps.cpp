#include <gtest/gtest.h>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>
#include <iostream>
// #include <cstdio> // remove - delete file

using namespace std;

constexpr int port = 8888;
constexpr int LOOP = 5;
string psudpbind = "udp://*:" + to_string(port);
string psudpaddr = "udp://127.0.0.1:"+to_string(port);
// string psunix = "unix://./unix.ps.uds";
unixaddr_t unaddr = unix_sockaddr();

struct psdata_t { int a; };

int i = 0;


void callback(const message_t& m) {
  psdata_t d = unpack<psdata_t>(m);
  EXPECT_EQ(d.a, i++);
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
  p.connect(addr);
  for (int i=0; i < LOOP; ++i) {
    psdata_t d{i};
    // message_t m = pack<psdata_t>(ps_test_data[i]);
    message_t m = pack<psdata_t>(d);
    p.publish(m);
    // marko::msleep(1);
  }
}

TEST(marko, pub_sub_udp) {
  i = 0;
  thread subth(sub_thread);
  marko::msleep(1);
  thread pubth(pub_thread);

  subth.join();
  pubth.join();
}

/////////////////////////////////////////

void sub_thread_un() {
  // unixaddr_t addr = unix_sockaddr(psunix);
  SubscriberUnix s(sizeof(psdata_t));
  s.bind(unaddr);

  s.register_cb( callback );
  for (int i=0; i < LOOP; ++i) {
    s.once();
  }
}

void pub_thread_un() {
  // unixaddr_t addr = unix_sockaddr(psunix);
  PublisherUnix p;
  // p.register_addr(unaddr);
  p.connect(unaddr);
  for (int i=0; i < LOOP; ++i) {
    psdata_t d{i};
    message_t m = pack<psdata_t>(d);
    p.publish(m);
    // marko::msleep(1);
  }
}

TEST(marko, pub_sub_unix) {
  i = 0;
  thread subth(sub_thread_un);
  marko::msleep(1);
  thread pubth(pub_thread_un);

  subth.join();
  pubth.join();
}
