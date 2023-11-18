#include "dataflow/graph.hpp"

namespace dataflow {

graph::graph(std::vector<node*> nodes) {
  for (auto n : nodes) {
    adj_list[n] = {};
    for (auto m : nodes) {
      if ((*n).input_connected_to(*m)) {
        adj_list[n].insert(m);
      }
    }
  }
}

const std::map<node*, std::set<node*>>& graph::adjacency() const {
  return adj_list;
}

void graph::dump(std::ostream& out) const {
  std::map<node*, int> node_ids;
  int counter = 0;
  for (auto&& [n, _] : adj_list) {
    node_ids[n] = counter++;
  }

  out << "digraph {\n";
  for (auto&& [n, _] : adj_list) {
    out << "  " << node_ids[n] << " [label=\"" << n->label()
        << "\", shape=\"box\"]\n";
  }
  for (auto&& [n, deps] : adj_list) {
    for (auto&& m : deps) {
      out << "  " << node_ids[m] << " -> " << node_ids[n] << "\n";
    }
  }
  out << "}" << std::endl;
}
}  // namespace dataflow