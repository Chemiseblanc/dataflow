#pragma once

#include <taskflow.hpp>

#include "dataflow/graph.hpp"
#include "dataflow/node.hpp"

namespace dataflow {
namespace adapters {

class taskflow : public virtual node {
 public:
  void operator()() override final {}
  void operator()(taskflow::Taskflow& tf) {}
};

}  // namespace adapters

inline void run_taskflow(tf::Executor& ex, graph& g) {
  tf::Taskflow& tf;

  ex.run(tf).wait();
}
inline void run_taskflow(graph& g) {
  tf::Executor ex;
  run_taskflow(ex, g);
}
}  // namespace dataflow