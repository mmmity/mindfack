#pragma once

// A finite automata with regular expressions on transitions
// It can only delete its vertices

#include "automata/nfa.hpp"

class RFA : NFA {
 public:

  RFA();
  RFA(const char* filename);
  RFA(const NFA& nfa);

  std::string minimize();

 private:
  void remove_vertex(size_t v);
};