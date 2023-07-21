#pragma once

#include "socket_udp.hpp"


class SocketBC: public SocketUDP {
  inetaddr_t bc_addr;

  public:
  SocketBC(uint16_t port) {
    // std::cout << "sock: " << socket_fd << std::endl;
    setsockopt(SOL_SOCKET, SO_BROADCAST, 1);
    setsockopt(IPPROTO_IP, IP_MULTICAST_LOOP, 1);
    // setsockopt(SOL_SOCKET, SO_BROADCAST, 1);
    bc_addr = inet_sockaddr("udp://bc:" + std::to_string(port));
    // std::cout << "bc_bind: " << get_ip_port(bc_addr) << std::endl;
  }
  ~SocketBC() {}

  void bind(const std::string&) = delete;
  void connect(const std::string&) = delete;

  inline int cast(const message_t& msg){
    return SocketUDP::sendto(msg, bc_addr);
  }
};