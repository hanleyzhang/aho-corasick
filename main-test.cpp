#include "ahocorasick.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>

int main(int argc, char *argv[]) {
  std::set<char> vocab;
  AhoCorasick algorithm(vocab);

  std::vector<std::string> ls;
  ls.push_back("he");
  ls.push_back("she");
  ls.push_back("hers");
  ls.push_back("his");
  assert(!algorithm.initialize(ls));

  std::vector<unsigned long> r;

  r = algorithm.parse("");
  assert(r.size() == 0);

  r = algorithm.parse("ushers");
  assert(r[0] == 1);
  assert(r[1] == 0);
  assert(r[2] == 2);
  assert(r.size() == 3);

  r = algorithm.parse("UsHeRs");
  assert(r[0] == 1);
  assert(r[1] == 0);
  assert(r[2] == 2);
  assert(r.size() == 3);

  std::cout << "PASSED" << std::endl;
}