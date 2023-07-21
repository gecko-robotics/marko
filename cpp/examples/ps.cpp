#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <array>
#include <marko/marko.hpp>

using namespace std;

string HOST = get_host_ip();
constexpr int PORT    = 3000;
// string address = "udp://" + HOST + ":" + to_string(PORT);
constexpr int MAX_LOOP = 10;

// publish data
struct data_t {
  double a;
  int b;
  void print() const {
    cout << "data_t " << a << " " << b << " size " << sizeof(*this) << endl;
  }
};


// publisher
void pub() {
  inetaddr_t addr = inet_sockaddr("udp://" + HOST + ":" + to_string(PORT));
  // cout << "Publisher connecting to: " << get_ip_port(addr) << endl;

  PublisherUDP p;
  // p.connect();
  p.register_addr(addr);

  for (int i = 0; i < MAX_LOOP; ++i) {
    data_t d;
    d.a = 1.2;
    d.b = i;
    message_t m = pack(d);
    p.publish(m);
  }
}

// subscriber callback, this would process
// recieved data from the publisher, but this
// only prints it.
void cb(const message_t &m) {
  data_t s = unpack<data_t>(m);
  s.print();
}

// subscriber
void sub() {
  SubscriberUDP s(sizeof(data_t));
  s.bind("udp://*:" + to_string(PORT));
  // s.bind(PORT);
  // s.settimeout(5000);
  s.register_cb(cb); // you can have many callback functions
  // s.register_cb(cb);
  for (int i=0; i < MAX_LOOP; ++i) s.once();
  // cout << "sub done" << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: ./ps p|s" << endl;
    return 1;
  }

  string arg(argv[1]);
  if (arg == "p") pub();
  else if (arg == "s") sub();
  else cout << "Usage: ./ps p|s" << endl;

  return 0;
}
