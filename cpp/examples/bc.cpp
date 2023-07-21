#include <iostream>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>

using namespace std;

constexpr uint16_t PORT = 11311;
constexpr int LOOP = 5;

void server() {
  SocketBC s(PORT);

  // SocketInfo si(s.getSocketFD());
  // si.info("server", SocketInfo::UDP);

  subscription_t ss{69, SUBSCRIBE};
  message_t sub = pack<subscription_t>(ss);

  for (int i=0; i<LOOP; ++i) {
    s.cast(sub);
    cout << "cast: " << sub << endl;
  }
}

void client() {
  SocketUDP s;
  s.reuseSocket(true);
  // s.bind(PORT);
  s.bind("udp://*:" + to_string(PORT));

  size_t sz = sizeof(subscription_t);

  for (int i=0; i<LOOP; ++i) {
    message_t m = s.recv(sz);
    subscription_t msg = unpack<subscription_t>(m);
    cout << int(msg.topic) << " " << int(msg.status) << endl;
  }
}

int main() {
  thread a(client);
  // thread b(client);
  marko::msleep(500);
  thread c(server);

  a.join();
  // b.join();
  c.join();
  return 0;
}