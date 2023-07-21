/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2019 Kevin Walchko
* see LICENSE for full details
\**************************************************/

#pragma once

#include "socket.hpp"









/*
 * Simple UDP socket base class for query/response architecture. Not really
 * useful by itself. Look at classes that build upon it.
 */
// class UDPSocket : public Socket {
// public:
//   // UDPSocket() : Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) {}
//   UDPSocket() {
//     // socket_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     // guard(socket_fd, "Socket() constructor failed");
//     // std::cout << "socket fd: " << this->socket_fd << std::endl;
//   }
//   ~UDPSocket() {}

//   // UDP bind
//   // INADDR_ANY - bind to all available interfaces
//   // port=0 let's the OS pick a port number
//   // void bind(const std::string &ip, int port) {

//   //   socket_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//   //   guard(socket_fd, "Socket() constructor failed");
//   //   // std::cout << "socket fd: " << this->socket_fd << std::endl;

//   //   // allow multiple sockets to re-use the same address and port
//   //   setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
//   //   setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);

//   //   geckoUDP_t addr = geckoUDP(ip, port);

//   //   int err = ::bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//   //   guard(err, "bind() failed for (" + ip + ":" + std::to_string(port) + ")");
//   //   // std::cout << "err: " << err << std::endl;

//   //   std::cout << "Binding to: " << ip << ":" << port << std::endl;
//   //   std::cout << "socket fd: " << this->socket_fd << std::endl;
//   // }

//   // void connect(const std::string &ip, int port) {
//   //   socket_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//   //   guard(socket_fd, "Socket() constructor failed");
//   //   // std::cout << "socket fd: " << this->socket_fd << std::endl;

//   //   geckoUDP_t addr = geckoUDP(ip, port);

//   //   int err = ::connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//   //   guard(err, "connect() failed for (" + ip + ":" + std::to_string(port) + ")");
//   //   // std::cout << "err: " << err << std::endl;

//   //   std::cout << "Connecting to: " << ip << ":" << port << std::endl;
//   //   std::cout << "socket fd: " << this->socket_fd << std::endl;
//   // }
// };



// // https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/srv.c
// // https://gist.github.com/Phaiax/ae7d1229e6f078457864dae712c51ae0
// class UDSSocket : public Socket {
// public:
//   // UDSSocket() : Socket(AF_UNIX, SOCK_STREAM, 0) {}
//   UDSSocket() {}

//   // UDS bind
//   void bind(const std::string &path) {
//     socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
//     guard(socket_fd, "Socket() constructor failed");

//     // struct sockaddr_un addr;
//     // memset(&addr, 0, sizeof(addr));
//     // addr.sun_family = AF_UNIX;
//     // strcpy(addr.sun_path, path.c_str());

//     geckoUDS_t addr = geckoUDS(path);

//     int err = ::bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//     guard(err, "bind() failed for UDS " + path);
//   }

//   void connect(const std::string &path) {
//     socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
//     guard(socket_fd, "Socket() constructor failed");

//     geckoUDS_t addr = geckoUDS(path);

//     int err = ::connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
//     guard(err, "connect() failed for UDS " + path);
//   }
// };
