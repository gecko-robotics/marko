#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <marko/marko.hpp>

using namespace std;

enum SocketType {
  UDS, UDP, BC, MC, TCP
};

class SockAddress {
  public:
  void filter(const std::string& addr) {

    regex proto("(udp|tcp|unix)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*,_,-,:]+)");
    smatch m;
    regex_search(addr, m, proto);

    if (m.size() == 0) {
      cout << "No matches!!" << endl;
    }
    else if (m[1] == "unix"){
      unixaddr_t ret{0};
      string path = m[2];
      cout << "unix path: " << path << endl;
      // ret = make_sockaddr(path);
    }
    else if (m[1] == "tcp" || m[1] == "udp") {
      sockaddr_in_t ret{0};
      regex ipport("([a-z,A-Z,\\d,\\/,.,*]+):([*,\\d]+)");
      smatch mm;
      string ss = m[2];
      regex_search(ss, mm, ipport);

      if (mm.size() != 3) cout << "error " << mm.size() << endl;
      string ip = mm[1];
      string sport = mm[2];
      uint16_t port = stoi(sport);

      // if (ip == "*"){
      //   ret = make_sockaddr(INADDR_ANY, port);
      // }
      // else if (ip == "bc"){
      //   ret = make_sockaddr(INADDR_BROADCAST, port);
      // }
      // else {
      //   ret = make_sockaddr(ip, port);
      // }

      // cout << get_ip_port(ret) << endl;
    }
  }
};

int main()
{
    string s{"udp://*:3000"};

    SockAddress sa;
    sa.filter(s);
    sa.filter("tcp://1.2.3.4:5000");
    sa.filter("unix:///bob/here");

  //  regex r("(udp|tcp|unix)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*]+):([\\d]+)");
  //  smatch m;
  //  regex_search(s, m, r);

    // for each loop
    // for (auto x : m)
    //     cout << x << endl;

    // cout << m[1] << endl;
    // cout << m[2] << endl;
    // cout << m[3] << endl;

    return 0;
}