#pragma once

#include <map>
#include <ostream>
#include <set>
#include <vector>

#include "dataflow/node.hpp"

namespace dataflow {
class graph {
 public:
  explicit graph(const std::vector<node*>& nodes);
  explicit graph(std::initializer_list<node*> nodes);

  [[nodiscard]] const std::map<node*, std::set<node*>>& adjacency() const;

  void dump(std::ostream& out) const;

 private:
  std::map<node*, std::set<node*>> adj_list;
};
}  // namespace dataflow
