/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
***************************************************
sa_family_t = uint16_t

struct sockaddr_un {
  sa_family_t sun_family;     // AF_UNIX
  char        sun_path[108];  // Pathname
};

struct sockaddr_in {
  sa_family_t    sin_family; // address family: AF_INET
  in_port_t      sin_port;   // port in network byte order
  struct in_addr sin_addr;   // internet address
};

// Internet address
struct in_addr {
  uint32_t       s_addr;     // address in network byte order
};
**************************************************/
#pragma once

#include <arpa/inet.h> // for sockaddr_in
#include <sys/types.h>  // for type definitions like size_t
#include <iostream>
#include <string>
// #include <sys/socket.h> // socket, connect, etc ...
#include <sys/un.h>     // UDS
#include <unistd.h>     // gethostname()
#include <netdb.h>      // gethostbyname
#include <regex>

constexpr uint32_t AF_ERROR = 100;

using sockaddr_t = struct sockaddr;
using sockaddr_in_t = struct sockaddr_in;
using inetaddr_t = struct sockaddr_in; // udp or tcp
using unixaddr_t = struct sockaddr_un; // uds

union SockAddr {
  inetaddr_t inet;
  unixaddr_t unix;
};


static
const SockAddr& filter(const std::string& address) {
  std::regex proto("(udp|tcp|unix)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*,_,-,:]+)");
  std::smatch m;

  // find [original, protocol, path|ip:port]
  regex_search(address, m, proto);
  SockAddr ans{0};
  ans.inet.sin_family = AF_ERROR;

  if (m.size() != 3) return std::move(ans);
  else if (m[1] == "unix"){
    std::string path = m[2];

    ans.unix.sun_family = AF_UNIX;
    strncpy(ans.unix.sun_path, path.c_str(), path.size());
    return std::move(ans);
  }
  else if (m[1] == "tcp" || m[1] == "udp") {
    uint16_t port;
    uint32_t ip;
    std::string ss = m[2];
    std::regex ipport("([a-z,A-Z,\\d,\\/,.,*]+):([*,\\d]+)");
    std::smatch mm;

    // find [original, ip, port]
    regex_search(ss, mm, ipport);
    if (mm.size() != 3) return std::move(ans);

    else if (mm[1] == "*") ip = INADDR_ANY;
    else if (mm[1] == "bc") ip = INADDR_BROADCAST;
    else ip = inet_addr(mm[1].str().c_str());

    if (mm[2] == "*") port = 0;
    else port = stoi(mm[2]);

    ans.inet.sin_family      = AF_INET;
    ans.inet.sin_addr.s_addr = ip;
    ans.inet.sin_port        = htons(port);
    return std::move(ans);
  }

  return std::move(ans);
}

const inetaddr_t& inet_sockaddr(const std::string &path) {
  return std::move(filter(path).inet);
}

const unixaddr_t& unix_sockaddr(const std::string &path) {
  return std::move(filter(path).unix);
}

/////////////////////////////////////////////////////////////////

static
std::string inet2string(const inetaddr_t &addr) {
  char ip[32] = {0};

  if (addr.sin_family == AF_INET) {
    ::inet_ntop(AF_INET, &(addr.sin_addr),ip,sizeof(ip));
    std::string host(ip,strlen(ip));
    host += ":" + std::to_string(ntohs(addr.sin_port));
    return host;
  }
  return "";
}

static
std::string unix2string(const unixaddr_t &addr) {
  char ip[32] = {0};

  if (addr.sun_family == AF_UNIX) {
    // ::inet_ntop(AF_INET, &(addr.sin_addr),ip,sizeof(ip));
    // std::string host(ip,strlen(ip));
    // host += ":" + std::to_string(ntohs(addr.sin_port));
    // std::cerr << "unix2string: " << addr.sun_path << std::endl;
    // std::string host(addr.sun_path, strlen(addr.sun_path));
    std::string host(addr.sun_path);
    return host;
  }
  return "";
}


static
std::ostream &operator<<(std::ostream &os, SockAddr const &s) {
  sa_family_t type = s.inet.sin_family;
  if (type == AF_INET) os << inet2string(s.inet);
  else if (type == AF_UNIX) os <<  s.unix.sun_path;
  else if (type == AF_ERROR) os << "ERROR";
  else os << "UNKNOWN";
  return os;
}

static
std::ostream &operator<<(std::ostream &os, unixaddr_t const &s) {
  os << "path: " << unix2string(s);
  return os;
}

static
std::ostream &operator<<(std::ostream &os, inetaddr_t const &s) {
  os << inet2string(s);
  return os;
}

/////////////////////////////////////////////////////////////////


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




// static
// std::string getHostbyName(const std::string& name) {
//   struct hostent* host = ::gethostbyname(name.c_str());
//   if (host == nullptr) return std::string("");
//   std::string hostname(host->h_addr, host->h_length);
//   return hostname;
// }

///////////////////////////////////////////////
// static
// udsaddr_t make_sockaddr(const std::string &path) {
//   udsaddr_t addr = {0};
//   addr.sun_family = AF_UNIX;
//   strcpy(addr.sun_path, path.c_str());
//   return std::move(addr);
// }

// static
// udpaddr_t make_sockaddr(const std::string &ip, uint16_t port) {
//   udpaddr_t addr = {0};
//   addr.sin_family      = AF_INET;
//   addr.sin_addr.s_addr = inet_addr(ip.c_str());
//   addr.sin_port        = htons(port);
//   return std::move(addr);
// }

// // inaddr: INADDR_ANY or INADDR_BROADCAST
// static
// udpaddr_t make_sockaddr(uint32_t inaddr, uint16_t port) {
//   udpaddr_t addr = {0};
//   addr.sin_family      = AF_INET;
//   addr.sin_addr.s_addr = inaddr; // htonl(inaddr); // htonl???
//   addr.sin_port        = htons(port);
//   return std::move(addr);
// }
///////////////////////////////////////////////


// // Gets geckoUDP_t for connected peer system.
// geckoUDP_t getpeername(int sockfd) {
//   geckoUDP_t addr      = {0};
//   unsigned int addrlen = sizeof(addr);
//   ::getpeername(sockfd, (struct sockaddr *)&addr, &addrlen);
//   return addr;
// }

// // Gets the geckoUDP_t for this system
// geckoUDP_t getsockname(int sockfd) {
//   geckoUDP_t addr      = {0};
//   unsigned int addrlen = sizeof(addr);
//   ::getsockname(sockfd, (struct sockaddr *)&addr, &addrlen);
//   return addr;
// }