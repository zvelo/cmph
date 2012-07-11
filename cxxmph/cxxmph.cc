// Copyright 2010 Google Inc. All Rights Reserved.
// Author: davi@google.com (Davi Reis)

#include <getopt.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "mph_map.h"
#include "config.h"

using std::cerr;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::string;
using std::vector;

using cxxmph::mph_map;

void usage(const char* prg) {
  cerr << "usage: " << prg << " [-v] [-h] [-V] <keys.txt>" << endl;
}
void usage_long(const char* prg) {
  usage(prg);
  cerr << "   -h\t print this help message" << endl;
  cerr << "   -V\t print version number and exit" << endl;
  cerr << "   -v\t increase verbosity (may be used multiple times)" << endl;
}

int main(int argc, char** argv) {

  int verbosity = 0;
  while (1) {
    char ch = (char)getopt(argc, argv, "hvV");
    if (ch == -1) break;
    switch (ch) {
      case 'h':
        usage_long(argv[0]);
        return 0;
      case 'V':
        std::cout << VERSION << std::endl;
        return 0;
      case 'v':
        ++verbosity;
        break;
    }
  }
  if (optind != argc - 1) {
    usage(argv[0]);
    return 1;
  }
  vector<string> keys;
  ifstream f(argv[optind]);
  if (!f.is_open()) {
    std::cerr << "Failed to open " << argv[optind] << std::endl;
    exit(-1);
  }
  string buffer;
  while (!getline(f, buffer).eof()) keys.push_back(buffer);
  for (uint32_t i = 0; i < keys.size(); ++i) string s = keys[i];
  mph_map<string, string> table;

  for (uint32_t i = 0; i < keys.size(); ++i) table[keys[i]] = keys[i];
  mph_map<string, string>::const_iterator it = table.begin();
  mph_map<string, string>::const_iterator end = table.end();
  for (int i = 0; it != end; ++it, ++i) {
    cout << i << ": " << it->first
         <<" -> " << it->second << endl;
  }
}
