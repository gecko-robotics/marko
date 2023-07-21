#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// you can send messages with:
// nc -u 10.71.6.23 6666

using namespace std;

constexpr size_t MAX_BUFFER_SIZE = 32;
string host{"127.0.0.1"};
// string host{"10.71.6.23"};
constexpr int port = 6666;
constexpr uint8_t MAX_LOOP = 5;
typedef struct sockaddr_in sockaddr_t;

sockaddr_t getUdpAddr(const string& ip, int port) {
  sockaddr_t addr = {0};
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip.c_str());
  addr.sin_port        = htons(port);
  return std::move(addr);
}

sockaddr_t getUdpAddr(int port) {
  sockaddr_t addr = {0};
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port        = htons(port);
  return std::move(addr);
}

std::string get_ip_port(const sockaddr_t &addr) {
  char ip[32] = {0};
  ::inet_ntop(AF_INET, &(addr.sin_addr),ip,sizeof(ip));
  std::string host(ip,strlen(ip));
  host += ":" + std::to_string(ntohs(addr.sin_port));
  return host;
}

std::string getsockname(const int socket_fd) {
    sockaddr_t addr = {0};
    socklen_t addr_len = sizeof(addr);
    int err = ::getsockname(socket_fd, (struct sockaddr*)&addr, &addr_len);
    return get_ip_port(addr);
  }

void info(int socket_fd){
  u_char val;
  socklen_t size = sizeof(val);
  string hostinfo = getsockname(socket_fd);

  printf("Socket =====================================\n");
  printf("  bind/connect to: %s\n", hostinfo.c_str());
  printf("  fd: %d\n", socket_fd);

  getsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, &size);
  printf("  multicast loopback: %s\n", val ? "enabled" : "disabled");

  getsockopt(socket_fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &size);
  printf("  accept connections: %s\n", val ? "enabled" : "disabled");

  getsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &val, &size);
  printf("  reuse addr: %s\n", val ? "true" : "false");

  getsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &val, &size);
  printf("  reuse port: %s\n", val ? "true" : "false");
}

//////////////////////////////////////////////////////////////

void sub() {
  int sockfd;
  sockaddr_t server_addr, client_addr;
  uint8_t buffer[MAX_BUFFER_SIZE];
  socklen_t addr_len;

  // Create UDP socket
  if ((sockfd = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  int val = 1;
  ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val));
  ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void *)&val, sizeof(val));

  server_addr = getUdpAddr(port);

  // Bind socket to the server address
  if (::bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Binding failed");
    exit(EXIT_FAILURE);
  }

  info(sockfd);

  // std::cout << "UDP sub is running and listening on port " << port << "..." << std::endl;

  for (int i=0; i < MAX_LOOP; i++) {
    memset(buffer, 0, sizeof(buffer));
    memset(&client_addr, 0, sizeof(client_addr));

    // Receive data from client
    addr_len = sizeof(client_addr);
    ssize_t num_bytes = ::recvfrom(sockfd, buffer, sizeof(buffer), 0,
    (struct sockaddr*)&client_addr, &addr_len);
    // ssize_t num_bytes = ::recv(sockfd, buffer, sizeof(buffer), 0);

    if (num_bytes < 0) {
      perror("Error in receiving data");
      exit(EXIT_FAILURE);
    }

    std::cout << "Received " << buffer << " from " << get_ip_port(client_addr) << std::endl;
  }

  // Close the socket
  close(sockfd);
}

void pub() {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  string buffer{"hello"};
  socklen_t addr_len;

  // Create UDP socket
  if ((sockfd = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  server_addr = getUdpAddr(host, port);
  addr_len = sizeof(server_addr);
  // if (::connect(sockfd, (struct sockaddr*)&server_addr, addr_len) < 0) {
  //   perror("Connect failed");
  //   exit(EXIT_FAILURE);
  // }
  info(sockfd);

  // std::cout << "UDP pub is running on port " << port << "..." << std::endl;

  for (uint8_t i=0; i < MAX_LOOP; i++) {

    // Reply to the client
    if (::sendto(sockfd, buffer.data(), buffer.size(), 0, (struct sockaddr*)&server_addr, addr_len) < 0) {
      perror("Error in sending data");
      exit(EXIT_FAILURE);
    }

    std::cout << "Reply sent to client " << std::endl;
  }

  // Close the socket
  close(sockfd);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: ./ps p|s" << endl;
    return 1;
  }

  string arg(argv[1]);
  if (arg == "p") pub();
  else if (arg == "s") sub();
  else cout << "Usage: ./ps p|s" << endl;

  return 0;
}