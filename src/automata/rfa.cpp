#include "automata/rfa.hpp"

RFA::RFA() : NFA() {}

RFA::RFA(const char* filename) : NFA(filename) {}

RFA::RFA(const NFA& nfa) : NFA(nfa) {}

void RFA::remove_vertex(size_t v) {
  if (terminal[v]) throw std::runtime_error("cannot remove non-terminal vertex");
  std::string loop_reg;
  std::vector<ToEdge> to_remove;
  for (const ToEdge& e : graph[v]) {
    if (e.to == v) {
      to_remove.push_back(e);
      if (e.by.empty()) continue;
      loop_reg += "(";
      loop_reg += e.by;
      loop_reg += ")";
      loop_reg += "|";
    }
  }

  for (const ToEdge& e : to_remove) {
    remove_edge(v, e);
  }

  if (!loop_reg.empty()) {
    loop_reg.pop_back();
    loop_reg = "(" + loop_reg + ")*";
  }
  for (const FromEdge& e : rev_graph[v]) {
    std::erase(graph[e.from], ToEdge{e.by, v});
    for (const ToEdge& e2 : graph[v]) {
      graph[e.from].push_back({e.by + loop_reg + e2.by, e2.to});
    }
  }

  for (const ToEdge& e : graph[v]) {
    std::erase(rev_graph[e.to], FromEdge{e.by, v});
    for (const FromEdge& e2 : rev_graph[v]) {
      rev_graph[e.to].push_back({e2.by + loop_reg + e.by, e2.from});
    }
  }

  graph[v].clear();
}

std::string RFA::minimize() {
  make_one_terminal_vertex();
  for (size_t i = 1; i < graph.size() - 1; ++i) {
    if (terminal[i]) continue;
    remove_vertex(i);
  }

  std::string final_regex;
  std::string final_loop;

  for (const ToEdge& e : graph[0]) {
    if (e.to == 0) {
      if (!e.by.empty()) final_loop += "(" + e.by + ")|";
    } else {
      if (e.by.empty()) final_regex += "(#)|";
      else final_regex += "(" + e.by + ")|";
    }
  }

  if (final_regex.empty() && final_loop.empty()) return "#";

  if (!final_regex.empty()) final_regex.pop_back();
  if (!final_loop.empty()) {
    final_loop.pop_back();
    final_regex = "(" + final_loop + ")*(" + final_regex + ")";
  }

  graph = {{{final_regex, 1}}, {}};
  rev_graph = {{}, {{final_regex, 0}}};
  terminal = {false, true};

  return final_regex;
}
