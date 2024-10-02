#include <iostream>
#include "regular/reg_parser.hpp"
#include "automata/dfa.hpp"
#include "automata/rfa.hpp"

std::vector<std::string> commands = {"reg_to_mindfa", "reg_to_complement"};

int main(int argc, char* argv[]) {
  if (argc < 3 || std::find(commands.begin(), commands.end(), argv[1]) == commands.end()) {
    std::cout << "Usage: mindfack [reg_to_mindfa|reg_to_complement] <regexp>";
    exit(1);
  }

  std::string regexp = argv[2];
  std::string command = argv[1];

  if (command == "reg_to_mindfa") {
    DFA out = RegParser::parse(regexp)->get_nfa();
    out.minimize();
    out.visualize(std::cout);
  } else if (command == "reg_to_complement") {
    DFA out = RegParser::parse(regexp)->get_nfa();
    out.minimize();
    out.make_full();
    out.make_complement();
    RFA rout = out.get_nfa();
    std::cout << rout.minimize();
  }
}