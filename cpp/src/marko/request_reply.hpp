/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
////////////////////////////////////////////////////////
//       Request / Reply
////////////////////////////////////////////////////////
#pragma once


#include "socket.hpp"
#include <functional> // std::function
#include <string>
#include <vector>

/*
UDS
----------------------------------
in_req.uds: req binds for recv()
in_rep.uds: rep binds for recv()

req.bind(in_req.uds)
rep.bind(in_rep.uds)

req.sendto(in_rep.uds) -> rep.recv()
rep.sendto(in_req.uds) -> req.recv()
*/

// #ifndef __RR_HPP__
// #define __RR_HPP__

template <typename SOCKET, typename SOCKADDR>
class Reply : public SOCKET {
public:
  Reply(size_t size): msg_size(size) {}
  ~Reply() {}

  typedef std::function<message_t(const message_t &)> ReplyCallback_t;

  inline void register_cb(ReplyCallback_t func) { callback = func; }

  // void loop(Event &event) { while (event.is_set()) once(); }
  // void loop() { while (true) once(); }

  bool once() {
    SOCKADDR from_addr = {0};
    message_t m = SOCKET::recvfrom(msg_size, &from_addr);
    if (m.size() == 0 || m.size() != msg_size) return false;
    message_t r = callback(m);
    SOCKET::sendto(r, from_addr);
    return true;
  }
  // void once();

  /*
  I don't like this, how does this know the "to" address? Maybe it is in
  the request for UDS?
  */
  bool once(const SOCKADDR& to) {
    message_t m = SOCKET::recv(msg_size); // bind()
    if (m.size() == 0 || m.size() != msg_size) return false;
    message_t r = callback(m);

    // makeSocket(AF_UNIX, SOCK_DGRAM, 0);
    // bind(to)
    SOCKET::sendto(r, to);
    return true;
  }

protected:
  ReplyCallback_t callback;
  const size_t msg_size;
};

using ReplyUDP = Reply<SocketUDP, inetaddr_t>;
using ReplyUnix = Reply<SocketUnix, unixaddr_t>; // broke

// template<>
// void ReplyUnix::once() {
//   unixaddr_t from_addr = {0};
//   message_t m = SocketUnix::recvfrom(msg_size, &from_addr);
//   if (m.size() == 0 || m.size() != msg_size) return;
//   message_t r = callback(m);
//   SocketUnix::sendto(r, from_addr);
// }

// template<>
// void ReplyUDP::once() {}

// /////////////////////////////////////////////////////////////////

template <typename SOCKET, typename SOCKADDR>
class Request : public SOCKET {
public:
  Request(size_t size): msg_size(size) {}
  ~Request() {}

  message_t request(const message_t &msg, const SOCKADDR &addr) {
    SOCKET::sendto(msg, addr);
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

// #endif