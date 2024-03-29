/**************************************************
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
***************************************************/
#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
// #include "sockaddr.hpp"
#include <unistd.h>     // gethostname()
#include <netdb.h>      // gethostbyname


// https://beej.us/guide/bgnet/html/multi/gethostbynameman.html
// PLEASE NOTE: these two functions are superseded by getaddrinfo() and
// getnameinfo()! In particular, gethostbyname() doesn't work well with IPv6.
static
std::string get_hostname() {
  char name[256] = {0};
  int flags = 0;
  inetaddr_t addr = inet_sockaddr("udp://127.0.0.1:9000");
  // guard(::gethostname(name, 256), "gethostname() failed");
  // ::gethostname(name, 256);
  int result = ::getnameinfo(
    (const sockaddr_t*)&addr, sizeof(addr),
    name, 265, // host name
    NULL, 0,  // service name ... don't care
    flags);
  return std::string(name);
}

static
std::string get_host_ip() {
  // Create a socket
  int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    // throw std::runtime_error("get_host_ip error");
    return "";
  }

  // you need to connect to get your IP address, even
  // a failed connect works
  inetaddr_t addr  = inet_sockaddr("udp://9.9.9.9:9999");
  ::connect(sockfd, (sockaddr_t*)&addr, sizeof(addr));

  // Get the local address
  socklen_t addrlen;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addrlen              = sizeof(addr);
  if (::getsockname(sockfd, (sockaddr_t *)&addr, &addrlen) < 0) {
    // throw std::runtime_error("get_host_ip error");
    return "";
  }

  // Convert the address to a string
  std::string address(inet_ntoa(addr.sin_addr));

  // Close the socket
  close(sockfd);

  return address;
}


class SocketInfo {
  public:
  SocketInfo(int fd): socket_fd(fd) {}

  // remove?
  enum SocketTypes {
    UDP, UDS, TCP
  };

  // SO_PROTOCOL: AF_INET, AF_UNIX ... doesn't work on macOS
  // SO_TYPE: SOCK_DGRAM, SOCK_STREAM
  void info(const std::string& s, SocketTypes st) {
    printf("[%s Socket] =====================================\n", s.c_str());
    printf("  bind/connect to: %s\n", getsockname().c_str());
    printf("  file descriptor: %d\n", socket_fd);

    int type;
    // int proto;
    socklen_t length = sizeof(type);

    ::getsockopt(socket_fd, SOL_SOCKET, SO_TYPE, &type, &length );
    // ::getsockopt(socket_fd, SO_PROTOCOL, SO_TYPE, &proto, &length );


    if (type == SOCK_DGRAM || type == SOCK_STREAM) info_so();
    if (type == SOCK_DGRAM) info_ip();
    // if (type == SOCK_DGRAM); // nothing yet
    // if (proto == AF_UNIX) printf("");
  }

  private:
  int socket_fd;

  void info_so(){
    u_char val;
    socklen_t size = sizeof(val);

    // printf("[%s Socket] =====================================\n", s.c_str());
    // printf("  bind/connect to: %s\n", getsockname().c_str());
    // printf("  file descriptor: %d\n", socket_fd);
    printf(" [Socket Layer]-----------------\n");

    for (const auto& m : socketOptsSOL) {
      getsockopt(socket_fd, SOL_SOCKET, m.val, &val, &size);
      if (m.type == dataTypes::Int)
        printf("  %s: %d\n", m.description.c_str(), int(val));
      else if (m.type == dataTypes::Bool)
        printf("  %s: %s\n", m.description.c_str(), val ? "true" : "false");
      else if (m.type == dataTypes::Type)
        printf("  %s: %s\n", m.description.c_str(), socketTypes[val].c_str());
    }
  }

  void info_ip(){
    // https://man7.org/linux/man-pages/man7/ip.7.html
    // https://man7.org/linux/man-pages/man7/tcp.7.html
    // https://man7.org/linux/man-pages/man7/udp.7.html
    u_char val;
    struct in_addr addr = {0};
    socklen_t ss = sizeof(addr);
    socklen_t size = sizeof(val);
    char ip[32] = {0};

    // Socket::info(s);

    printf(" [IP Protocol]-----------------\n");
    getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &addr, &ss);
    ::inet_ntop(AF_INET, &(addr.s_addr), ip, sizeof(ip));
    printf("  multicast IF: %s\n", ip);

    getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, &size);
    printf("  multicast loopback: %s\n", val ? "enabled" : "disabled");

    getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &val, &size);
    printf("  multicast TTL: %d\n", val);

    getsockopt(socket_fd, IPPROTO_IP, IP_TTL, &val, &size);
    printf("  IP TTL: %d\n", val);
  }

  std::string getsockname() {
    inetaddr_t addr = {0};
    socklen_t addr_len = sizeof(addr);
    int err = ::getsockname(socket_fd, (struct sockaddr*)&addr, &addr_len);
    // guard(err, "getsockname(): ");
    if (err < 0) return std::string();
    return to_string(addr);
  }

  std::map<int,std::string> socketTypes {
    {SOCK_DGRAM, "DGRAM"},
    {SOCK_RAW, "RAW"},
    {SOCK_STREAM, "STREAM"}
  };

  enum dataTypes {
    Int,
    Bool,
    Type,
    Timeval
  };

  struct SoOpts {
    int val;
    std::string description;
    dataTypes type;
  };

  std::vector<SoOpts> socketOptsSOL {
    {SO_TYPE, "socket type", dataTypes::Type},
    {SO_ACCEPTCONN, "Accepting connections", dataTypes::Bool}, // int
    {SO_BROADCAST, "Broadcast messages supported", dataTypes::Bool}, // int
    // SO_DEBUG, Debugging information is being recorded,
    {SO_DONTROUTE, "Bypass normal routing", dataTypes::Bool},
    {SO_ERROR, "Socket error status", dataTypes::Bool}, // int
    // {SO_KEEPALIVE, "Connections are kept alive with periodic messages", dataTypes::Bool}, // int
    // {SO_LINGER, "Socket lingers on close", dataTypes::Bool}, // linger struct
    {SO_OOBINLINE, "Out-of-band data is transmitted in line", dataTypes::Bool},
    // {SO_RCVTIMEO, "receive timeout"}, // timeval
    {SO_REUSEADDR, "SO_REUSEADDR", dataTypes::Bool},
    {SO_REUSEPORT, "SO_REUSEPORT", dataTypes::Bool},
    {SO_RCVBUF, "Receive buffer size", dataTypes::Int},
    {SO_RCVLOWAT, "receive low water mark", dataTypes::Int},
    {SO_SNDBUF, "Send buffer size", dataTypes::Int},
    {SO_SNDLOWAT, "send low water mark", dataTypes::Int},
    // {SO_SNDTIMEO, "send timeout"}, // timeval
  };
};
