// /**************************************************\
// * The MIT License (MIT)
// * Copyright (c) 2014 Kevin Walchko
// * see LICENSE for full details
// \**************************************************/
// ////////////////////////////////////////////////////////
// //       Request / Reply
// ////////////////////////////////////////////////////////
#pragma once


#include "event.hpp"  // Event
#include "socket_types.hpp"
#include "sockaddr.hpp"
#include <functional> // std::function
#include <string>
#include <vector>


template <typename SOCKET, typename SOCKADDR>
class Reply : public SOCKET {
public:
  Reply(size_t size): msg_size(size) {}
  ~Reply() {}

  typedef std::function<message_t(const message_t &)> ReplyCallback_t;

  inline void register_cb(ReplyCallback_t func) { callback = func; }

  void loop(Event &event) { while (event.is_set()) once(); }
  void loop() { while (true) once(); }

  void once() {
    SOCKADDR from_addr = {0};
    message_t m = SOCKET::recvfrom(msg_size, &from_addr);

    // std::cerr << "from_addr: " << from_addr << std::endl;
    // std::cerr << "m.size(): " << m.size() << " " << msg_size << std::endl;

    if (m.size() == 0 || m.size() != msg_size) return;

    message_t r = callback(m);

    // std::cerr << "r.size(): " << r.size() << std::endl;

    SOCKET::sendto(r, from_addr);
    // std::cerr << "once done" << std::endl;
  }

protected:
  ReplyCallback_t callback;
  const size_t msg_size;
};

using ReplyUDP = Reply<SocketUDP, inetaddr_t>;
using ReplyUnix = Reply<SocketUnix, unixaddr_t>;

// /////////////////////////////////////////////////////////////////

template <typename SOCKET, typename SOCKADDR>
class Request : public SOCKET {
public:
  Request(size_t size): msg_size(size) {}
  ~Request() {}

  message_t request(const message_t &msg, const SOCKADDR &addr) {
    SOCKET::sendto(msg, addr);

    // std::cerr << "waiting for reply" << std::endl;

    // SOCKADDR from_addr = {0};
    // message_t rep = SOCKET::recvfrom(msg_size, &from_addr);
    message_t rep = SOCKET::recv(msg_size);
    return std::move(rep);
  }

protected:
  const size_t msg_size;
};

using RequestUDP = Request<SocketUDP, inetaddr_t>;
using RequestUnix = Request<SocketUnix, unixaddr_t>;