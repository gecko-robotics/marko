#include <cstdio>
#include <iostream>
#include <marko/marko.hpp>
#include <string>
#include <vector>

using namespace std;

string HOST = get_host_ip();
constexpr uint16_t PORT = 9999;
constexpr int LOOP = 5;

// request message
struct __attribute__((packed)) request_t {
  double a;
  int b;
};

// response message
struct __attribute__((packed)) response_t {
  int a;
};

// requester
void request() {
  inetaddr_t addr = inet_sockaddr("udp://" + HOST + ":" + to_string(PORT));
  cout << "Request connecting to: " << inet2string(addr) << endl;

  RequestUDP r(sizeof(response_t));
  // r.connect();


  SocketInfo si(r.getSocketFD());
  si.info("Connect", SocketInfo::UDP);

  for (int i=0; i<LOOP; ++i) {
    request_t m{1.2345*double(i), i};
    message_t msg = pack<request_t>(m);
    message_t rp = r.request(msg, addr);
    response_t resp = unpack<response_t>(rp);
    cout << "resp: " << resp.a << endl;
  }
}

// response callback, this processes the
// request message and returns a response message
message_t cb(const message_t &m) {
  request_t s = unpack<request_t>(m);
  cout << "cb: " << s.a << " " << s.b << " " << sizeof(s) <<endl;

  response_t r{s.b};
  message_t resp = pack<response_t>(r);

  return std::move(resp);
}

// reply
void reply() {
  cout << "Reply binding to: " << HOST << ":" << PORT << endl;
  ReplyUDP r(sizeof(request_t));
  r.bind("udp://*:" + to_string(PORT));
  // r.bind(PORT);
  // r.settimeout(1000);

  SocketInfo si(r.getSocketFD());
  si.info("Bind", SocketInfo::UDP);

  r.register_cb(cb); // you can have more than 1 callback
  for (int i=0; i<LOOP; ++i) r.once();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: ./rr reply|request" << endl;
    return 1;
  }

  string arg(argv[1]);
  if (arg == "reply") reply();
  else if (arg == "request") request();
  else cout << "Usage: ./rr reply|request" << endl;

  return 0;
}
