#pragma once

#include <unordered_map>
#include <unordered_set>
#include "automata/base_automata.hpp"
#include "automata/nfa.hpp"

class DFA : public BaseAutomata {
 public:
  DFA();
  DFA(const char* filename);
  DFA(NFA nfa);

  bool allows(const std::string& word) const override;
  size_t size() const override;
  size_t edges() const;

  bool is_full();
  void make_full();
  void minimize();
  void make_complement();

  NFA get_nfa();

  DFA& operator=(const DFA&) = default;

  void visualize(std::ostream&) const override;

 private:
  void thompsons_algorithm(const NFA& nfa);
  std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> split(
      const std::unordered_set<size_t>&, const std::unordered_set<size_t>&,
      char) const;

  std::vector<std::unordered_map<char, size_t>> graph;
  std::vector<bool> terminal;
};