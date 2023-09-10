#include <gtest/gtest.h>
#include <marko/marko.hpp>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

using namespace std;

constexpr int port = 9999;
constexpr int LOOP = 5;
string udpbind = "udp://*:" + to_string(port);
string udpaddr = "udp://127.0.0.1:"+to_string(port);
string unix = "unix://./unix.rr.uds";
string unix2 = "unix://./unix.rr2.uds";


struct __attribute__((packed)) request_t { int a; };
typedef request_t response_t;

// response callback, this processes the
// request message and returns a response message
const message_t cb2(const message_t &m) {
  request_t req = unpack<request_t>(m);

  response_t resp{2*req.a};
  message_t msg = pack<response_t>(resp);

  // cout << "req*2: " << 2*req.a << " resp: " << resp.a << endl;

  return msg;
}

////////////////////////////////////////////////////////////

// requester
void req_thread() {
  inetaddr_t addr = inet_sockaddr(udpaddr);

  RequestUDP r(sizeof(response_t));

  for (int i=0; i<LOOP; ++i) {
    request_t d{i};
    message_t msg = pack<request_t>(d);
    message_t rp = r.request(msg, addr);
    response_t resp = unpack<response_t>(rp);

    EXPECT_EQ(resp.a, 2*d.a);
    marko::msleep(1);
  }
}

// reply
void rep_thread() {
  inetaddr_t addr = inet_sockaddr(udpaddr);
  ReplyUDP r(sizeof(request_t));
  r.bind(addr);

  r.register_cb(cb2); // you can have more than 1 callback
  for (int i=0; i<LOOP; ++i) r.once();
}

TEST(marko, req_rep_udp) {
  // for (int i=0; i<LOOP; ++i) test_data.push_back({i});
  thread repth(rep_thread);
  marko::msleep(1);
  thread reqth(req_thread);

  reqth.join();
  repth.join();
}

//////////////////////////////////////////////////////////////
// requester - doesn't work
/*
req.bind(unix)
req.r
*/
void req_thread_un() {
  unixaddr_t addr = unix_sockaddr(unix);
  unixaddr_t to_reply = unix_sockaddr(unix2);

  RequestUnix r(sizeof(response_t));
  r.bind(addr);

  // SocketInfo si(r.getSocketFD());
  // si.info("Connect", SocketInfo::UDP);

  for (int i=0; i<LOOP; ++i) {
    request_t req{i};
    message_t msg = pack<request_t>(req);
    message_t rp = r.request(msg, to_reply);
    response_t resp = unpack<response_t>(rp);

    EXPECT_EQ(resp.a, 2*req.a);
    EXPECT_EQ(rp.size(), sizeof(resp));

    marko::msleep(1);
  }
}

// reply
void rep_thread_un() {
  unixaddr_t to_request = unix_sockaddr(unix);
  unixaddr_t addr = unix_sockaddr(unix2);
  ReplyUnix r(sizeof(request_t));
  r.bind(addr);

  r.register_cb(cb2); // you can have more than 1 callback
  for (int i=0; i<LOOP; ++i) r.once(to_request);
}

TEST(marko, req_rep_unix) {
  thread repth(rep_thread_un);
  marko::msleep(1);
  thread reqth(req_thread_un);

  reqth.join();
  repth.join();
}
