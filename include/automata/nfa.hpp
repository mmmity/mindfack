#pragma once

#include <algorithm>
#include <deque>
#include <fstream>
#include <numeric>
#include <vector>
#include "automata/base_automata.hpp"

class RFA;

class NFA : public BaseAutomata {
 public:
  struct ToEdge {
    std::string by;
    size_t to;
    bool operator==(const ToEdge& other) const = default;
  };

  struct FromEdge {
    std::string by;
    size_t from;
    bool operator==(const FromEdge& other) const = default;
  };

  NFA();
  NFA(const char* filename);
  void remove_empty_edges();
  void expand_edges();
  bool allows(const std::string&) const override;
  void visualize(std::ostream&) const override;
  int add_vertex(const std::vector<FromEdge>& from,
                 const std::vector<ToEdge>& to, bool term);
  size_t size() const override;
  size_t edges() const;
  bool has_empty_edges() const;
  bool has_long_edges() const;

  NFA(const NFA& other) = default;
  NFA& operator=(const NFA& other) = default;

  void make_one_terminal_vertex();
  void parallel(NFA other);
  void consecutive(NFA other);
  void klini_asterisk();
  void klini_plus();

  size_t terminal_count() const;

 protected:
  std::vector<std::vector<ToEdge>> graph{};
  std::vector<std::vector<FromEdge>> rev_graph{};
  std::vector<bool> terminal{};

  void load_from_file(const char* filename);
  void remove_edge(size_t v, const ToEdge& e);
  void add_edge(size_t v, ToEdge e);
  void expand_edge(size_t v, const ToEdge& edge);

  friend class DFA;
};