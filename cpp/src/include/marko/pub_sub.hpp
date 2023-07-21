/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
////////////////////////////////////////////////////////
//       Publisher / Subscriber
////////////////////////////////////////////////////////
#pragma once

// #include "base.hpp"
#include "event.hpp"
#include "socket.hpp"
#include "socket_types.hpp"
#include "signals.hpp"
#include "sockaddr.hpp"
#include <functional> // std::function
#include <string>
#include <vector>

template <typename SOCKET>
class Subscriber : public SOCKET {
public:
  Subscriber(size_t data_size): data_size(data_size) {}
  ~Subscriber() {}

  typedef std::function<void(const message_t &)> SubCallback_t;

  void register_cb(SubCallback_t func) { callbacks.push_back(func); }

  void loop() { while (true) once(); }
  void loop(Event &event) { while (event.is_set()) once(); }

  void once() {
    message_t msg = SOCKET::recv(data_size);
    if (msg.size() == 0) return;
    for (const SubCallback_t &callback: this->callbacks) callback(msg);
  }

protected:
  std::vector<SubCallback_t> callbacks; // FIXME: rename callbacks
  size_t data_size;
};

using SubscriberUDP = Subscriber<SocketUDP>;
using SubscriberUnix = Subscriber<SocketUnix>;

///////////////////////////////////////////////////////////////////////////

template <typename SOCKET, typename SOCKADDR>
class Publisher : public SOCKET {
public:
  Publisher() {}
  ~Publisher() {}

  void publish(const message_t &data) {
    for (const SOCKADDR &addr : clientaddrs) SOCKET::sendto(data, addr);
  }

  inline void register_addr(const SOCKADDR &c) { clientaddrs.push_back(c); }

protected:
  std::vector<SOCKADDR> clientaddrs;
};

using PublisherUDP = Publisher<SocketUDP, inetaddr_t>;
using PublisherUnix = Publisher<SocketUnix, unixaddr_t>;