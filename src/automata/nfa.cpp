#include "automata/nfa.hpp"
#include "automata/rfa.hpp"

NFA::NFA() : graph(1), rev_graph(1), terminal(1, false) {}

void NFA::load_from_file(const char* filename) {
  std::ifstream fin(filename);
  size_t n, m;
  fin >> n >> m;
  graph.resize(n);
  rev_graph.resize(n);
  for (int i = 0; i < m; ++i) {
    size_t u, v;
    std::string by;
    fin >> u >> v >> by;
    if (by == "#")
      by.pop_back();  // # means eps-transition
    graph[u].push_back({by, v});
    rev_graph[v].push_back({by, u});
  }
  terminal.resize(n, false);
  int terminal_n;
  while (fin >> terminal_n) {
    terminal[terminal_n] = true;
  }
  fin.close();
}

NFA::NFA(const char* filename) {
  load_from_file(filename);
}

int NFA::add_vertex(const std::vector<FromEdge>& from,
                    const std::vector<ToEdge>& to, bool term) {
  size_t v_index = graph.size();
  graph.push_back(std::vector<ToEdge>());
  rev_graph.push_back(std::vector<FromEdge>());
  for (const FromEdge& edge : from) {
    graph[edge.from].push_back({edge.by, v_index});
    rev_graph[v_index].push_back(edge);
  }
  for (const ToEdge& edge : to) {
    if (edge.to == v_index)
      continue;
    graph[v_index].push_back(edge);
    rev_graph[edge.to].push_back({edge.by, v_index});
  }
  terminal.push_back(term);
  return v_index;
}

void NFA::remove_edge(size_t v, const ToEdge& e) {
  std::erase(graph[v], e);
  std::erase(rev_graph[e.to], FromEdge{e.by, v});
}

void NFA::add_edge(size_t v, ToEdge e) {
  rev_graph[e.to].push_back({e.by, v});
  graph[v].push_back(std::move(e));
}

void NFA::expand_edge(size_t v, const ToEdge& e) {
  size_t n = e.by.size();
  size_t last_vertex = v;
  for (size_t i = 0; i < static_cast<int>(n - 2); ++i) {
    last_vertex =
        add_vertex({{std::string(1, e.by[i]), last_vertex}}, {}, false);
  }
  add_vertex({{std::string(1, e.by[n - 2]), last_vertex}},
             {{std::string(1, e.by[n - 1]), e.to}}, false);
  remove_edge(v, e);
}

void NFA::expand_edges() {
  for (size_t v = 0; v < graph.size(); ++v) {
    std::vector<ToEdge> saved_edges;
    for (const ToEdge& edge : graph[v]) {
      if (edge.by.size() >= 2)
        saved_edges.push_back(edge);
    }
    for (const ToEdge& edge : saved_edges) {
      expand_edge(v, edge);
    }
  }
}

void NFA::remove_empty_edges() {
  size_t n = graph.size();
  std::vector<std::vector<bool>> eps_graph_matrix(n,
                                                  std::vector<bool>(n, false));
  for (size_t v = 0; v < n; ++v) {
    for (const ToEdge& e : graph[v]) {
      if (e.by.empty()) {
        eps_graph_matrix[v][e.to] = true;
      }
    }
  }

  // transitive closure
  for (size_t k = 0; k < n; ++k) {
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        eps_graph_matrix[i][j] =
            eps_graph_matrix[i][j] |
            (eps_graph_matrix[i][k] && eps_graph_matrix[k][j]);
      }
    }
  }

  // add new terminal states
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (eps_graph_matrix[i][j] && terminal[j])
        terminal[i] = true;
    }
  }

  // add new edges
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (eps_graph_matrix[i][j]) {
        for (const ToEdge& edge : graph[j]) {
          if (!edge.by.empty() && std::find(graph[i].begin(), graph[i].end(),
                                            edge) == graph[i].end()) {
            graph[i].push_back(edge);
            rev_graph[edge.to].push_back({edge.by, i});
          }
        }
      }
    }
  }

  // remove empty edges
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (eps_graph_matrix[i][j]) {
        remove_edge(i, {"", j});
      }
    }
  }
}

bool NFA::allows(const std::string& word) const {
  size_t n = graph.size(), m = word.size();
  std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n, false));
  for (int i = 0; i < n; ++i) {
    dp[m][i] = terminal[i];
  }

  for (int word_pos = m; word_pos >= 0; --word_pos) {
    if (word_pos !=
        m) {  // We want to handle eps-transitions even for the base of dp
      for (int v = 0; v < n; ++v) {
        for (const ToEdge& e : graph[v]) {
          if (!e.by.empty() && m - word_pos >= e.by.size() &&
              word.substr(word_pos, e.by.size()) == e.by) {
            dp[word_pos][v] =
                dp[word_pos][v] || dp[word_pos + e.by.size()][e.to];
          }
        }
      }
    }

    // now we need to handle eps-transitions with BFS
    std::vector<bool> used(n);
    std::deque<size_t> queue;
    for (size_t i = 0; i < n; ++i) {
      if (dp[word_pos][i]) {
        queue.push_back(i);
      }
    }
    while (!queue.empty()) {
      size_t current_v = queue.front();
      queue.pop_front();
      for (const FromEdge& edge : rev_graph[current_v]) {
        if (edge.by.empty() && !dp[word_pos][edge.from]) {
          dp[word_pos][edge.from] = true;
          queue.push_back(edge.from);
        }
      }
    }
  }

  return dp[0][0];
}

size_t NFA::terminal_count() const {
  return std::accumulate(terminal.begin(), terminal.end(), 0);
}

void NFA::make_one_terminal_vertex() {
  if (terminal_count() == 1 && terminal.back()) return;
  size_t vertex_id = graph.size();
  graph.push_back({});
  rev_graph.push_back({});
  for (size_t i = 0; i < terminal.size(); ++i) {
    if (terminal[i]) {
      graph[i].push_back({"", vertex_id});
      rev_graph[vertex_id].push_back({"", i});
      terminal[i] = false;
    }
  }
  terminal.push_back(true);
}

void NFA::parallel(NFA other) {
  make_one_terminal_vertex();
  other.make_one_terminal_vertex();
  std::vector<std::vector<ToEdge>> new_graph(other.size() + size() + 2);
  std::vector<std::vector<FromEdge>> new_rev_graph(other.size() + size() + 2);
  new_graph[0].push_back({"", 1});
  new_rev_graph[1].push_back({"", 0});
  new_graph[0].push_back({"", size() + 1});
  new_rev_graph[size() + 1].push_back({"", 0});

  for (size_t i = 0; i < graph.size(); ++i) {
    if (terminal[i]) {
      new_graph[i + 1].push_back({"", size() + other.size() + 1});
      new_rev_graph[size() + other.size() + 1].push_back({"", i + 1});
    }
    for (const ToEdge& e : graph[i]) {
      new_graph[i + 1].push_back({e.by, e.to + 1});
      new_rev_graph[e.to + 1].push_back({e.by, i + 1});
    }
  }

  for (size_t i = 0; i < other.graph.size(); ++i) {
    if (other.terminal[i]) {
      new_graph[i + 1 + size()].push_back({"", size() + other.size() + 1});
      new_rev_graph[size() + other.size() + 1].push_back({"", i + 1 + size()});
    }
    for (const ToEdge& e : other.graph[i]) {
      new_graph[i + 1 + size()].push_back({e.by, e.to + 1 + size()});
      new_rev_graph[e.to + 1 + size()].push_back({e.by, i + 1 + size()});
    }
  }

  terminal.assign(size() + other.size() + 2, false);
  terminal[size() + other.size() + 1] = true;
  graph = new_graph;
  rev_graph = new_rev_graph;
}

void NFA::consecutive(NFA other) {
  make_one_terminal_vertex();
  other.make_one_terminal_vertex();

  size_t current_size = size();
  graph.back().push_back({"", current_size});
  graph.resize(current_size + other.size());
  rev_graph.resize(current_size + other.size());
  rev_graph[current_size].push_back({"", current_size - 1});
  terminal.assign(current_size + other.size(), false);

  for (size_t i = 0; i < other.size(); ++i) {
    if (other.terminal[i]) terminal[i + current_size] = true;
    for (const ToEdge& e : other.graph[i]) {
      graph[i + current_size].push_back({e.by, e.to + current_size});
      rev_graph[e.to + current_size].push_back({e.by, i + current_size});
    }
  }
}

void NFA::klini_asterisk() {
  make_one_terminal_vertex();
  std::vector<std::vector<ToEdge>> new_graph(size() + 1);
  std::vector<std::vector<FromEdge>> new_rev_graph(size() + 1);
  for (size_t i = 0; i < graph.size(); ++i) {
    if (terminal[i]) {
      new_graph[i + 1].push_back({"", 0});
      new_rev_graph[0].push_back({"", i + 1});
    }
    for (const ToEdge& e : graph[i]) {
      new_graph[i + 1].push_back({e.by, e.to + 1});
      new_rev_graph[e.to + 1].push_back({e.by, i + 1});
    }
  }
  new_graph[0].push_back({"", 1});
  new_rev_graph[1].push_back({"", 0});

  terminal.assign(size() + 1, false);
  terminal[0] = true;
  graph = new_graph;
  rev_graph = new_rev_graph;
}

void NFA::klini_plus() {
  NFA asterisk = *this;
  asterisk.klini_asterisk();
  consecutive(asterisk);
}

size_t NFA::size() const {
  return graph.size();
}

size_t NFA::edges() const {
  size_t ans = 0;
  for (int i = 0; i < graph.size(); ++i)
    ans += graph[i].size();
  return ans;
}

bool NFA::has_empty_edges() const {
  for (size_t i = 0; i < graph.size(); ++i) {
    for (const ToEdge& e : graph[i]) {
      if (e.by.empty())
        return true;
    }
  }
  return false;
}

bool NFA::has_long_edges() const {
  for (size_t i = 0; i < graph.size(); ++i) {
    for (const ToEdge& e : graph[i]) {
      if (e.by.size() > 1)
        return true;
    }
  }
  return false;
}

void NFA::visualize(std::ostream& out) const {
  out << graph.size() << " ";
  size_t edges = 0;
  for (int i = 0; i < graph.size(); ++i)
    edges += graph[i].size();
  out << edges << "\n";
  for (int i = 0; i < graph.size(); ++i) {
    for (const ToEdge& e : graph[i]) {
      out << i << " " << e.to << " " << e.by << "\n";
    }
  }

  out << "terminal: ";
  for (int i = 0; i < graph.size(); ++i) {
    if (terminal[i])
      out << i << " ";
  }
}