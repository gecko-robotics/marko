/**************************************************
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
***************************************************/
#pragma once


#include "socket.hpp"


class SocketUDP: public Socket {
  public:
  SocketUDP() { makeSocket(AF_INET, SOCK_DGRAM, 0); }
  ~SocketUDP() {}

  bool bind(const inetaddr_t& addr) {
    int err = ::bind(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
    // guard(err, "SocketUDP::bind() failed: ");
    return err == 0 ? true : false;
  }

  bool connect(const inetaddr_t& addr) {
    if (reuseSocket(true) == false) return false;
    return bind(addr);
  }

  // void connect(const std::string&) = delete;

  message_t recvfrom(size_t msg_size, inetaddr_t *from_addr, const int flags=0) {
    message_t dst(msg_size);
    int num = 0;

    if (from_addr != NULL) {
      socklen_t from_len = sizeof(*from_addr);
      memset(from_addr, 0, from_len);
      num = ::recvfrom(socket_fd,
        dst.data(), msg_size,
        flags,
        (sockaddr_t*)from_addr, &from_len);
    }
    else
      num = ::recvfrom(socket_fd, dst.data(), msg_size, flags, NULL, NULL);

    // std::cout << "recvfrom done msg: " << dst.capacity() << " " << dst.size() << std::endl;

    // FIXME: add msg_size != dst.size() ???
    // timeout gives -1, so set size=0
    if (num == SOCKET_TIMEOUT || num == 0) dst.clear();

    return std::move(dst);
  }

  message_t recv(size_t msg_size, const int flags=0) {
    message_t m = recvfrom(msg_size,NULL,flags);
    return std::move(m);
  }

  int sendto(const message_t& msg, const inetaddr_t &addr, int flags=0) {
    // std::cout << "sendto" << std::endl;
    int num = ::sendto(
      socket_fd,
      msg.data(), msg.size(),
      flags,
      (struct sockaddr *)&addr, sizeof(addr));

    // guard(msg.size() != num, "sendto() sent incorrect number of bytes");
    // std::cout << "sendto done" << std::endl;
    return num;
  }

  std::string getsockname() {
    inetaddr_t addr = {0};
    socklen_t addr_len = sizeof(addr);
    int err = ::getsockname(socket_fd, (sockaddr_t*)&addr, &addr_len);
    // guard(err, "getsockname(): ");
    if (err != 0) return std::string("");
    return to_string(addr);
  }
};
