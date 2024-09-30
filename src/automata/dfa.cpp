#include "automata/dfa.hpp"

DFA::DFA() : graph(1), terminal(1, false) {}

DFA::DFA(const char* filename) {
  std::ifstream fin(filename);
  size_t n, m;
  fin >> n >> m;
  graph.resize(n);

  for (int i = 0; i < m; ++i) {
    size_t u, v;
    char c;
    fin >> u >> v >> c;
    if (graph[u].count(c) > 0 && graph[u][c] != v) {
      throw std::runtime_error("Automata must be deterministic!!!");
    }
    graph[u][c] = v;
  }

  terminal.resize(n, false);
  size_t terminal_v;
  while (fin >> terminal_v) {
    terminal[terminal_v] = true;
  }
}

DFA::DFA(NFA nfa) : graph(1), terminal(1, false) {
  nfa.expand_edges();
  nfa.remove_empty_edges();
  thompsons_algorithm(nfa);
}

void DFA::thompsons_algorithm(const NFA& nfa) {
  int n = nfa.size();
  if (n >= 64) {
    throw std::runtime_error(
        "this implementation only works for small automatons, less than 64 "
        "vertices");
  }
  std::deque<size_t> states_to_add = {1};
  std::unordered_map<size_t, size_t> number_in_dfa;
  std::unordered_set<size_t> used;
  used.insert(1);
  number_in_dfa[1] = 0;
  while (!states_to_add.empty()) {
    size_t added_state = states_to_add.front();
    states_to_add.pop_front();
    for (char c = 0; c >= 0; ++c) {
      size_t state_to_move = 0;
      for (int i = 0; i < n; ++i) {
        if (added_state & (1 << i)) {
          for (const NFA::ToEdge& e : nfa.graph[i]) {
            if (e.by[0] == c)
              state_to_move |= (1 << e.to);
          }
        }
      }
      if (state_to_move == 0)
        continue;
      if (number_in_dfa.count(state_to_move) == 0) {
        graph.push_back({});
        terminal.push_back(false);
        number_in_dfa[state_to_move] = number_in_dfa.size();
      }
      graph[number_in_dfa[added_state]][c] = number_in_dfa[state_to_move];
      if (used.count(state_to_move) == 0) {
        states_to_add.push_back(state_to_move);
        used.insert(state_to_move);
      }
    }
  }

  // making some vertices terminal
  for (auto [subset, number] : number_in_dfa) {
    for (int i = 0; i < n; ++i) {
      if ((subset & (1 << i)) > 0 && nfa.terminal[i]) {
        terminal[number] = true;
        break;
      }
    }
  }
}

bool DFA::allows(const std::string& word) const {
  size_t current_vertex = 0;
  for (char c : word) {
    if (graph[current_vertex].count(c) == 0) {
      return false;
    }
    current_vertex = graph[current_vertex].at(c);
  }
  return terminal[current_vertex];
}

size_t DFA::size() const {
  return graph.size();
}

size_t DFA::edges() const {
  size_t ans = 0;
  for (size_t i = 0; i < graph.size(); ++i) {
    ans += graph[i].size();
  }
  return ans;
}

bool DFA::is_full() {
  std::unordered_set<char> alphabet;
  for (int i = 0; i < graph.size(); ++i) {
    for (auto [by, to] : graph[i]) {
      alphabet.insert(by);
    }
  }

  for (int i = 0; i < graph.size(); ++i) {
    for (char c : alphabet) {
      if (graph[i].count(c) == 0)
        return false;
    }
  }

  return true;
}

void DFA::make_full() {
  if (is_full())
    return;

  std::unordered_set<char> alphabet;
  for (int i = 0; i < graph.size(); ++i) {
    for (auto [by, to] : graph[i]) {
      alphabet.insert(by);
    }
  }

  graph.push_back({});
  terminal.push_back(false);
  for (char c : alphabet) {
    graph.back()[c] = graph.size() - 1;
    for (size_t i = 0; i < graph.size(); ++i) {
      if (graph[i].count(c) == 0) {
        graph[i][c] = graph.size() - 1;
      }
    }
  }
}

std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> DFA::split(
    const std::unordered_set<size_t>& uset,
    const std::unordered_set<size_t>& splitter, char c) const {
  std::unordered_set<size_t> r1, r2;
  for (size_t v : uset) {
    if (splitter.contains(graph[v].at(c)))
      r1.insert(v);
    else
      r2.insert(v);
  }

  return {r1, r2};
}

void DFA::minimize() {
  // This algorithm is O(n^2) in time, faster algorithm exists
  make_full();
  std::vector<std::unordered_set<size_t>> partition(2);
  for (int i = 0; i < terminal.size(); ++i) {
    if (terminal[i])
      partition[1].insert(i);
    else
      partition[0].insert(i);
  }
  std::deque<std::pair<std::unordered_set<size_t>, char>> queue;
  for (auto [c, unused_] : graph[0]) {
    queue.push_back({partition[0], c});
    queue.push_back({partition[1], c});
  }

  while (!queue.empty()) {
    auto [uset, c] = queue.front();
    queue.pop_front();
    for (const std::unordered_set<size_t>& r : partition) {
      auto [r1, r2] = split(r, uset, c);
      if (!r1.empty() && !r2.empty()) {
        std::erase(partition, r);
        partition.push_back(r1);
        partition.push_back(r2);
        for (auto [c, unused_] : graph[0]) {
          queue.push_back({r1, c});
          queue.push_back({r2, c});
        }
      }
    }
  }
  for (size_t pi = 0; pi < partition.size(); ++pi) {
    if (partition[pi].contains(0)) {
      partition[0].swap(partition[pi]);
      break;
    }
  }

  std::vector<size_t> partition_number(graph.size());
  for (size_t pi = 0; pi < partition.size(); ++pi) {
    for (size_t v : partition[pi]) {
      partition_number[v] = pi;
    }
  }

  size_t new_n = partition.size();
  std::vector<std::unordered_map<char, size_t>> new_graph(new_n);
  std::vector<bool> new_terminal(new_n, false);
  for (int i = 0; i < graph.size(); ++i) {
    for (auto [c, v] : graph[i]) {
      new_graph[partition_number[i]][c] = partition_number[v];
    }
    if (terminal[i])
      new_terminal[partition_number[i]] = true;
  }

  graph = new_graph;
  terminal = new_terminal;
}

void DFA::make_complement() {
  make_full();
  for (size_t i = 0; i < terminal.size(); ++i) {
    terminal[i] = !terminal[i];
  }
}

void DFA::visualize(std::ostream& out) const {
  out << size() << " " << edges() << "\n";
  for (size_t i = 0; i < graph.size(); ++i) {
    for (auto [c, v] : graph[i]) {
      out << i << " " << v << " " << c << "\n";
    }
  }

  for (int i = 0; i < terminal.size(); ++i) {
    if (terminal[i])
      out << i << " ";
  }
  out << "\n";
}