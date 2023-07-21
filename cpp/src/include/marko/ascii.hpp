/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <exception>

using ascii_t = std::vector<std::string>;

/*
 The multicast beacon uses a simple ASCII encoding for message traffic. why?
 The messages tend to be small and you waste more time encoding/decoding
 when you use something fancier. Also, SDP uses ASCII and that works.

 Ex: (key|topic|pid) -> dalek|camera|13412

 */
class Ascii {
public:
  Ascii(char separator = '|') : sep(separator) {}

  ascii_t &unpack(const std::string &str) {
    toks.clear();
    std::stringstream ss(str);
    std::string token;
    try {
      while (getline(ss, token, sep))
        toks.push_back(token);
    } catch (std::exception &e) {
      // std::cout << e.what() << std::endl;
      toks.clear();
      return toks;
    }
    return toks;
  }

  std::string pack(ascii_t &v) {
    std::stringstream ss;
    ss << v[0];
    for (int i = 1; i < v.size(); i++)
      ss << sep << v[i];
    return ss.str();
  }

protected:
  ascii_t toks;
  char sep;
};
