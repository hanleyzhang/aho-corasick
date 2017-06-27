#include <iostream>
#include "ahocorasick.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

void print_result(std::vector<std::string> &keywords, std::vector<unsigned long>& idx) {
  if (idx.size() == 0) {
    std::cout << "none" << std::endl;
  } else {
    for (const unsigned long& i: idx) {
      std::cout << keywords[i] << std::endl;
    }
  }
  //std::cout << std::endl;
}

int main(int argc, char *argv[]) {

  // rudimentary argument parser
  if ((argc != 2 and argc != 3) or (argc > 1 and !strcmp(argv[1], "-h"))) {
    std::cout << "Usage: " << argv[0] << " dictionary-file [quoted-test-string]" << std::endl;
    return 0;
  }

  // initialize the algorithm
  std::string vocab_str = "abcdefghijklmnopqrstuvwxyz";
  std::set<char> vocab;
  for (auto c: vocab_str) {
    vocab.insert(c);
  }
  AhoCorasick algorithm(vocab);

  // read the keywords, initialize the algorithm
  std::vector<std::string> ls;
  std::ifstream infile(argv[1]);
  std::string line;
  while (std::getline(infile, line)) {
    ls.push_back(line);
  }
  std::cout << "Read " << ls.size() << " keywords from " << argv[1] << std::endl;

  if (algorithm.initialize(ls)) {
    std::cout << "Failed to initialize" << std::endl;
    return 1;
  }

  if (argc == 3) {
    // only parse the given string
    std::cout << "Parse: " << argv[2] << std::endl;
    std::vector<unsigned long> r = algorithm.parse(argv[2]);
    print_result(ls, r);
  } else {
    // parse interactively
    std::cout << "Press Ctrl+C when you are bored." << std::endl;

    while (true) {
      std::string input;
      std::cout << "Input string: ";
      std::getline(std::cin, input);
      if (input.size() == 0) {
        break;
      }
      std::vector<unsigned long> r = algorithm.parse(input);
      print_result(ls, r);
      //std::cout << std::endl;
    }
  }
  return 0;
}