#pragma once

#include <functional>
#include <vector>

#include "dataflow/graph.hpp"

namespace dataflow {
inline void run_serial(graph& g) {
  auto adj = g.adjacency();
  // topological sort
  std::vector<node*> sorted;
  std::map<node*, bool> visited;

  std::function<void(node*)> topo_sort = [&adj, &visited, &sorted,
                                          &topo_sort](node* n) {
    if (visited[n]) return;

    visited[n] = true;

    auto& deps = adj.at(n);
    for (auto m : deps) {
      if (!visited[m]) {
        topo_sort(m);
      }
    }

    sorted.push_back(n);
  };

  for (auto&& [n, deps] : adj) {
    topo_sort(n);
  }

  // run in order
  for (auto& n : sorted) {
    (*n)();
  }
}
}  // namespace dataflow