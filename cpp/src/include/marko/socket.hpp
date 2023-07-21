/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
#pragma once

#include "time.hpp"
#include "sockaddr.hpp"
#include "message.hpp"
#include <errno.h> // errno
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h> // socket(), bind(), recvfrom(), ...
// #include <sys/un.h>     // UDS
#include <netinet/in.h> // for address structs
#include <unistd.h>     // for close()
// #include <map>
#include <regex>

extern int errno; // don't like this global value

constexpr int SOCKET_ERR = -1;
constexpr int SOCKET_TIMEOUT = -1;
constexpr int SOCKET_OK = 0;


class Socket {
  protected:
  typedef int socket_fd_t;
  socket_fd_t socket_fd; // socket file descriptor

public:
  Socket(): socket_fd(SOCKET_ERR) {}
  ~Socket() { close(); }

  void close() {
    if (socket_fd != SOCKET_ERR) {
      ::close(socket_fd);
      socket_fd = SOCKET_ERR;
    }
  }

  socket_fd_t getSocketFD() { return socket_fd; }

  void settimeout(long timeout_msec) {
    timeval_t tv = get_time(timeout_msec);

    int err = ::setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    guard(err, "settimeout(): ");
  }

  void setnonblocking(bool val) {
    // int flags = guard(fcntl(socket_fd, F_GETFL), "setblocking(): ");
    // guard(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK), "setblocking(): ");

    int on = val ? 1 : 0; // FIONBIO - enable nonblocking
    guard(ioctl(socket_fd, FIONBIO, (char *)&on), "setnonblocking(): ");
  }

  // level: IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IP, SOL_SOCKET
  void setsockopt(int level, int name, int val) {
    int err = ::setsockopt(socket_fd, level, name, (char *)&val, sizeof(val));
    guard(err, "setsockopt(): ");
  }

  // FIXME: move to UDP
  std::string getsockname() {
    inetaddr_t addr = {0};
    socklen_t addr_len = sizeof(addr);
    int err = ::getsockname(socket_fd, (sockaddr_t*)&addr, &addr_len);
    guard(err, "getsockname(): ");
    return inet2string(addr);
  }

  // bool available(long msec=1){
  bool available(){
    timeval_t tv{0,1000}; // 1 msec (1000 usec)
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(socket_fd, &readfds);

    // select searchs up to not including socket+1
    // don't care about writefds and exceptfds:
    int err = select(socket_fd+1, &readfds, NULL, NULL, &tv);
    guard(err, "Socket::available select issue");

    if (FD_ISSET(socket_fd, &readfds)) return true;
    return false;
  }

  bool availableForWrite(){
    timeval_t tv{0,1000}; // 1 msec (1000 usec)
    fd_set writefds;

    FD_ZERO(&writefds);
    FD_SET(socket_fd, &writefds);

    // select searchs up to not including socket+1
    // don't care about readfds and exceptfds:
    int err = select(socket_fd+1, NULL, &writefds, NULL, &tv);
    guard(err, "Socket::availableForWrite select issue");

    if (FD_ISSET(socket_fd, &writefds)) return true;
    return false;
  }

  void reuseSocket(bool enable) {
    // allow multiple sockets to re-use the same address and port
    if (enable) {
      setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
      setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
    }
    else {
      setsockopt(SOL_SOCKET, SO_REUSEPORT, 0);
      setsockopt(SOL_SOCKET, SO_REUSEADDR, 0);
    }
  }

  void bind(const std::string& address) { setSocket(address, BIND); }
  void connect(const std::string& address) { setSocket(address, CONNECT); }

protected:
  void makeSocket(int family, int type, int proto) {
    socket_fd = ::socket(family, type, proto);
    guard(socket_fd, "Socket::makeSocket() failed: ");
  }

  inline void guard(int err, const std::string &msg) {
    if (err < 0) {
      this->close();
      std::cout << msg + std::string(strerror(int(errno))) << std::endl;
      throw std::runtime_error(msg + std::string(strerror(int(errno))));
    }
  }

  enum ConType: uint8_t {
    CONNECT,
    BIND
  };

  void setSocket(const std::string& address, const ConType type) {
    std::regex proto("(udp|tcp|unix)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*,_,-,:]+)");
    std::smatch m;
    regex_search(address, m, proto);

    if (m.size() == 0) {
      guard(-1, "Socket UDS address invalide: " + address);
    }
    else if (m[1] == "unix"){
      unixaddr_t addr = unix_sockaddr(address);

      std::string path(m[2]);
      unlink(path.c_str());
      int err = 0;
      if (type == CONNECT) {
        err = ::connect(socket_fd, (const sockaddr_t*)&addr, sizeof(addr));
        guard(err, "Socket UDS couldn't connect: ");
      }
      else if (type == BIND) {
        err = ::bind(socket_fd, (const sockaddr_t*)&addr, sizeof(addr));
        guard(err, "Socket UDS couldn't bind: ");
      }
      else guard(-1, "Socket UDS neither connect or bind");
    }
    else if (m[1] == "tcp" || m[1] == "udp") {
      sockaddr_in_t addr = inet_sockaddr(address);

      int err = 0;
      if (type == CONNECT) err = ::connect(socket_fd, (const sockaddr_t *)&addr, sizeof(addr));
      else if (type == BIND) err = ::bind(socket_fd, (const sockaddr_t *)&addr, sizeof(addr));
      else err = -1;
      guard(err, "Socket::bind() failed: ");
    }
  }
};











////////////////////////////////////////////////////////////////////////////////////////


// class SocketUDP: public Socket {
//   public:
//   SocketUDP() {}
//   ~SocketUDP() {}

//   void bind(const std::string &ip, int port, bool multipleBind=false) {
//     get_socket(AF_INET, SOCK_DGRAM, 0);

//     // allow multiple sockets to re-use the same address and port
//     if (multipleBind) {
//       setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
//       setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
//     }
//     else {
//       setsockopt(SOL_SOCKET, SO_REUSEPORT, 0);
//       setsockopt(SOL_SOCKET, SO_REUSEADDR, 0);
//     }

//     inetaddr_t addr;
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family      = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY; // all interfaces
//     addr.sin_port        = htons(port);

//     int err = ::bind(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
//     guard(err, "bind() failed: ");

//     // info("Bind");
//   }

//   void connect(const std::string &ip, int port) {
//     get_socket(AF_INET, SOCK_DGRAM, 0);
//     // info("Connect");
//   }

//   message_t recvfrom(size_t msg_size, inetaddr_t *from_addr, const int flags=0) {
//     message_t dst(msg_size);
//     int num = 0;

//     if (from_addr != NULL) {
//       socklen_t from_len;
//       from_len = sizeof(*from_addr);
//       memset(from_addr, 0, from_len);
//       num = ::recvfrom(socket_fd, dst.data(), msg_size, flags,
//         (struct sockaddr *)from_addr, &from_len);
//     }
//     else
//       num = ::recvfrom(socket_fd, dst.data(), msg_size, flags, NULL, NULL);

//     // std::cout << "recvfrom done msg: " << dst.capacity() << " " << dst.size() << std::endl;

//     // FIXME: add msg_size != dst.size() ???
//     // timeout gives -1, so set size=0
//     if (num == SOCKET_TIMEOUT || num == 0) dst.clear();

//     return std::move(dst);
//   }

//   message_t recv(size_t msg_size, const int flags=0) {
//     message_t m = recvfrom(msg_size,NULL,flags);
//     return std::move(m);
//   }

//   int sendto(const message_t& msg, const inetaddr_t &addr, int flags=0) {
//     int num = ::sendto(
//       socket_fd,
//       msg.data(),
//       msg.size(),
//       flags,
//       (struct sockaddr *)&addr,
//       sizeof(addr));

//     guard(msg.size() != num, "sendto() sent incorrect number of bytes");
//     return num;
//   }

//   // static inetaddr_t getsockaddr(const int port) {
//   //   inetaddr_t addr = {0};
//   //   addr.sin_family      = AF_INET;
//   //   addr.sin_addr.s_addr = INADDR_ANY;
//   //   addr.sin_port        = htons(port);
//   //   return std::move(addr);
//   // }

//   // static inetaddr_t getsockaddr(const std::string& ip, int port) {
//   //   inetaddr_t addr = {0};
//   //   addr.sin_family      = AF_INET;
//   //   addr.sin_addr.s_addr = inet_addr(ip.c_str());
//   //   addr.sin_port        = htons(port);
//   //   return std::move(addr);
//   // }

//   void info(const std::string& s){
//     // https://man7.org/linux/man-pages/man7/ip.7.html
//     // https://man7.org/linux/man-pages/man7/tcp.7.html
//     // https://man7.org/linux/man-pages/man7/udp.7.html
//     u_char val;
//     struct in_addr addr = {0};
//     socklen_t ss = sizeof(addr);
//     socklen_t size = sizeof(val);
//     char ip[32] = {0};

//     Socket::info(s);

//     printf(" [IP Protocol]-----------------\n");
//     getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &addr, &ss);
//     ::inet_ntop(AF_INET, &(addr.s_addr), ip, sizeof(ip));
//     printf("  multicast IF: %s\n", ip);

//     getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, &size);
//     printf("  multicast loopback: %s\n", val ? "enabled" : "disabled");

//     getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &val, &size);
//     printf("  multicast TTL: %d\n", val);

//     getsockopt(socket_fd, IPPROTO_IP, IP_TTL, &val, &size);
//     printf("  IP TTL: %d\n", val);
//   }
// };

// class SocketUDS: public Socket {
//   public:
//   SocketUDS() {}
//   ~SocketUDS() {}

//   void bind(const std::string &path, bool multipleBind=false) {}
//   message_t recvfrom(size_t msg_size, unixaddr_t *from_addr, const int flags=0) {
//     message_t m;
//     return std::move(m);
//   }
//   message_t recv(size_t msg_size, const int flags=0) {
//     message_t m;
//     return std::move(m);
//   }
//   int sendto(const message_t& msg, const unixaddr_t &addr, int flags=0) {
//     return 0;
//   }

//   static unixaddr_t getsockaddr(const std::string& path) {
//     unixaddr_t addr = {0};
//     // addr.sin_family      = AF_INET;
//     // addr.sin_addr.s_addr = inet_addr(ip.c_str());
//     // addr.sin_port        = htons(port);
//     return std::move(addr);
//   }
// };











// template<>
// void Socket<struct sockaddr_in>::bind(const std::string &ip, int port) {

//     socket_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     guard(socket_fd, "Socket() constructor failed");
//     // std::cout << "socket fd: " << this->socket_fd << std::endl;

//     // allow multiple sockets to re-use the same address and port
//     setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
//     setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);

//     // SOCKADDR addr = geckoUDP(ip, port);
//     // geckoUDP addr = geckoUDP(ip, port);
//     struct sockaddr_in addr = geckoUDP(ip, port);

//     int err = ::bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//     guard(err, "bind() failed for (" + ip + ":" + std::to_string(port) + ")");
//     // std::cout << "err: " << err << std::endl;

//     std::cout << "Binding to: " << ip << ":" << port << std::endl;
//     std::cout << "socket fd: " << this->socket_fd << std::endl;
//   }
















// [DEPRECATED]
// msgaddr_t recvfrom() {
//   // clear the receive buffers & structs
//   memset(recv_str, 0, sizeof(recv_str)); // clear buffer

//   sockaddr_t from_addr  = {0};
//   unsigned int from_len = sizeof(from_addr);
//   int recv_len          = 0;

//   // block waiting to receive a packet
//   recv_len = ::recvfrom(socket_fd, recv_str, MAX_LEN, 0,
//                         (struct sockaddr *)&from_addr, &from_len);
//   // if (recv_len < 0) {
//   //   // throw SocketError("recvfrom() failed");
//   //   throw SocketError();
//   // }
//   guard(recv_len < 0, "recvfrom() failed");

//   return msgaddr_t({recv_str}, from_addr);
// }

// if timeout set, use select to determine if data ready
// if (/*timeout >*/ 0) {
//     struct timeval tv;
//     fd_set readfds;
//     long sec = 0;
//     long msec = (long)timeout;
//
//     if (msec >= 1000) {
//         sec = msec/1000;
//         msec %= 1000;
//     }
//
//     tv.tv_sec = sec;
//     tv.tv_usec = msec * 1000;
//
//     FD_ZERO(&readfds);
//     FD_SET(socket_fd, &readfds);
//
//     // don't care about writefds and exceptfds:
//     int status = select(sock+1, &readfds, NULL, NULL, &tv);
//     if (status == 0) return false;
//     else if (status < 0) guard(status, "recvfrom(): ");
//
//     // if (FD_ISSET(socket_fd, &readfds) == 0) {
//     //     // cout << "no data" << endl;
//     //     return false;
//     // }
//     // cout << "data!!" << endl;
// }

// class Base {
// public:
//   Base() { ; }

//   void info() { ; }

//   inline void bind(const std::string &addr, int port) {
//     socket.bind(addr, port);
//   }

// protected:
//   UDPSocket socket;
// };

// value?
// bool ready() {
//   struct timeval tv;
//   tv.tv_sec  = 0;
//   tv.tv_usec = 1000;

//   fd_set readfds;
//   FD_ZERO(&readfds);
//   FD_SET(socket_fd, &readfds); // get sock info

//   // don't care about writefds and exceptfds:
//   int status = select(sock + 1, &readfds, NULL, NULL, &tv);
//   if (status == 0) return false;
//   else if (status < 0) guard(status, "recvfrom(): ");
//   return true;
// }

/**
 * Handles old C socket functions and throws SocketError with error description
 * from errno.
 */
// int guard(int err, std::string msg) {
//   if (err < 0) {
//     throw SocketError(msg + std::string(strerror(int(errno))));
//   }
//   return err;
// }

// sockaddr_t make(const string &saddr, int port) {
//   sockaddr_t addr;
//   memset(&addr, 0, sizeof(addr));
//   addr.sin_family      = AF_INET;
//   addr.sin_addr.s_addr = inet_addr(saddr.c_str());
//   addr.sin_port        = htons(port);
//   // cout << ">> Addr: " << inet_ntoa(addr.sin_addr) << ":"
//   // << ntohs(addr.sin_port) << endl;
//   return std::move(addr);
// }

// struct sockaddr_in make(int port, int iaddr){
//     struct sockaddr_in addr = {0};
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = htonl(iaddr);
//     return std::move(addr);
// }


// // UDS bind
// void bindUDS(const std::string &path) {
//   struct sockaddr_un addr;
//   memset(&addr, 0, sizeof(addr));
//   addr.sun_family = AF_UNIX;
//   strcpy(addr.sun_path, path.c_str());

//   int err = ::bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//   guard(err, "bind() failed for UDS");
// }