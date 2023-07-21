
#include <gtest/gtest.h>
#include <iostream>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>

using namespace std;

constexpr uint16_t PORT = 11311;
constexpr int LOOP = 5;
subscription_t bc_test_data[LOOP]{
  {19,SUBSCRIBE},
  {72,UNSUBSCRIBE},
  {103,SUBSCRIBE},
  {144,UNSUBSCRIBE},
  {250,SUBSCRIBE}
};

void server() {
  SocketBC s(PORT);

  for (int i=0; i<LOOP; ++i) {
    message_t sub = pack<subscription_t>(bc_test_data[i]);
    s.cast(sub);
    marko::msleep(100);
  }
}

void client() {
  SocketUDP s;
  s.reuseSocket(true);
  s.bind("udp://*:" + to_string(PORT));

  size_t sz = sizeof(subscription_t);

  for (int i=0; i<LOOP; ++i) {
    message_t m = s.recv(sz);
    subscription_t msg = unpack<subscription_t>(m);
    // cerr << int(msg.topic) << " " << int(msg.status) << endl;
    EXPECT_EQ(msg.topic, bc_test_data[i].topic);
    EXPECT_EQ(msg.status, bc_test_data[i].status);
  }
}

TEST(marko, broadcast_socket) {
  thread a(client);
  thread b(client);
  marko::msleep(500);
  thread c(server);

  a.join();
  b.join();
  c.join();
}