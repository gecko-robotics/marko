#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>
#include <marko/marko.hpp>
#include <regex>

using namespace std;

// constexpr uint32_t AF_ERROR = 100;

// union SockInfo {
//   sockaddr_in_t inet;
//   udsaddr_t unix;
// };

// static
// std::ostream &operator<<(std::ostream &os, SockInfo const &s) {
//   sa_family_t type = s.inet.sin_family;
//   if (type == AF_INET) os << get_ip_port(s.inet);
//   else if (type == AF_UNIX) os <<  s.unix.sun_path;
//   else if (type == AF_ERROR) os << "ERROR";
//   else os << "UNKNOWN";
//   return os;
// }

// static
// const SockInfo& filter(const std::string& address) {
//   std::regex proto("(udp|tcp|unix)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*,_,-,:]+)");
//   std::smatch m;

//   // find [original, protocol, path|ip:port]
//   regex_search(address, m, proto);
//   SockInfo ans{0};
//   ans.inet.sin_family = AF_ERROR;

//   if (m.size() != 3) return std::move(ans);
//   else if (m[1] == "unix"){
//     std::string path = m[2];

//     ans.unix.sun_family = AF_UNIX;
//     strncpy(ans.unix.sun_path, path.c_str(), path.size());
//     return std::move(ans);
//   }
//   else if (m[1] == "tcp" || m[1] == "udp") {
//     uint16_t port;
//     uint32_t ip;
//     std::string ss = m[2];
//     std::regex ipport("([a-z,A-Z,\\d,\\/,.,*]+):([*,\\d]+)");
//     std::smatch mm;

//     // find [original, ip, port]
//     regex_search(ss, mm, ipport);
//     if (mm.size() != 3) return std::move(ans);

//     else if (mm[1] == "*") ip = INADDR_ANY;
//     else if (mm[1] == "bc") ip = INADDR_BROADCAST;
//     else ip = inet_addr(mm[1].str().c_str());

//     if (mm[2] == "*") port = 0;
//     else port = stoi(mm[2]);

//     ans.inet.sin_family      = AF_INET;
//     ans.inet.sin_addr.s_addr = ip;
//     ans.inet.sin_port        = htons(port);
//     return std::move(ans);
//   }

//   return std::move(ans);
// }

int main() {

  // SockInfo si;
  // string s{"udp://*:3000"};
  // si = filter(s);
  // cout << si << endl;
  // si = filter("tcp://1.2.3.4:5000");
  // cout << si << endl;
  // si = filter("unix:///bob/here");
  // cout << si << endl;
  // si = filter("unix://./bob/here.udp");
  // cout << si << endl;
  // si = filter("tcp:///bob/here");
  // cout << si << endl;
  // si = filter("tcps://1.2.3.4:5000");
  // cout << si << endl;

  return 0;
}