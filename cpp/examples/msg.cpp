#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <array>
#include <marko/marko.hpp>

using namespace std;

// using message_t = vector<uint8_t>;

struct data_t {
  double a;
  int b;
  void print() const {
    cout << "data_t " << a << " " << b << " size " << sizeof(*this) << endl;
  }
};

struct data2_t {
  double a;
  int b;
  void print() const {
    cout << "data_t " << a << " " << b << " size " << sizeof(*this) << endl;
  }
};

int main() {
  data_t d{1.1,3};
  data2_t dd{2.2,6};

  message_t m = pack(d);
  message_t mm = pack(dd); // compiler figers out template type

  for (const uint8_t &b: mm) cout << int(b) << ',';
  cout << endl;

  data_t r = unpack<data_t>(m);
  if (r.a == d.a && r.b == d.b) cout << "good: " << r.a << " " << r.b << endl;
  else cout << "bad" << endl;

  message_t s(1024);
  cout << "size of contructed: " << s.size() << endl;

  return 0;
}