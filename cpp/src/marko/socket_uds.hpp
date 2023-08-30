
#pragma once

#include "socket.hpp"
#include <unistd.h>  // unlink
#include <sys/un.h>  // UDS


class SocketUnix: public Socket {
  public:
  SocketUnix() { makeSocket(AF_UNIX, SOCK_DGRAM, 0); }
  ~SocketUnix() {}

  message_t recvfrom(size_t msg_size, unixaddr_t *from_addr, const int flags=0) {
    if (from_addr == NULL) return message_t();

    message_t dst(msg_size);

    socklen_t from_len = sizeof(*from_addr);
    // std::cerr << "from_addr != NULL " << from_len << std::endl;
    memset(from_addr, 0, from_len);
    int num = ::recvfrom(
      socket_fd,
      dst.data(), msg_size,
      flags,
      (sockaddr_t*)from_addr, &from_len);

    // guard(num, "Unix recvfrom: ");

    // FIXME: add msg_size != dst.size() ???
    // timeout gives -1, so set size=0
    if (num == SOCKET_TIMEOUT || num == 0) dst.clear();

    return std::move(dst);
  }

  message_t recv(size_t msg_size, const int flags=0) {
    message_t dst(msg_size);
    int num = ::recv(socket_fd, dst.data(), msg_size, flags);
    guard(num, "Unix recv: ");
    // std::cout << "recvfrom done msg: " << dst.capacity() << " " << dst.size() << std::endl;

    // FIXME: add msg_size != dst.size() ???
    // timeout gives -1, so set size=0
    if (num == SOCKET_TIMEOUT || num == 0) dst.clear();

    return std::move(dst);
  }

  int sendto(const message_t& msg, const unixaddr_t &addr, int flags=0) {
    int num = ::sendto(
      socket_fd,
      msg.data(), msg.size(),
      flags,
      (sockaddr_t*)&addr, sizeof(addr));

    guard(msg.size() != num, "sendto() sent incorrect number of bytes");
    // std::cerr << "UDS sendto " << msg.size() << std::endl;
    return num;
  }

  void connect(const std::string& address) = delete;

};