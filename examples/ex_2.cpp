#include <cstddef>
#include <iostream>

#include "dataflow/dataflow.hpp"

class constant_integer : public dataflow::outputs<int> {
 public:
  explicit constant_integer(const int value) { outputs::get<0>() = value; }
};
class multiply_by_two : public dataflow::inputs<int>,
                        public dataflow::outputs<int> {
 public:
  void operator()() { outputs::get<0>() = 2 * inputs::get<0>(); }
};
class print_integer : public dataflow::inputs<int> {
 public:
  void operator()() { std::cout << inputs::get<0>() << std::endl; }
};

class sum_range : public dataflow::inputs<dataflow::many<int>>,
                  public dataflow::outputs<int> {
 public:
  void operator()() {
    int accumulator = 0;
    for (auto&& value : inputs::get<0>()) {
      accumulator += value;
    }
    outputs::get<0>() = accumulator;
  }
};

class adder : public dataflow::inputs<int, int>, public dataflow::outputs<int> {
public:
  void operator()() {
    outputs::get<0>() = inputs::get<0>() + inputs::get<1>();
  }
};

int main(int argc, char* argv[]) {
  // Create dataflow objects
  constant_integer source{1};
  constant_integer second_source{2};
  sum_range node;
  adder second_node;
  print_integer sink;

  // Connect ports together
  node.inputs::connect<0>() = source.outputs::connect<0>();
  node.inputs::connect<0>() = second_source.outputs::connect<0>();
  second_node.inputs::connect<0>() = source.outputs::connect<0>();
  second_node.inputs::connect<1>() = node.outputs::connect<0>();
  sink.inputs::connect<0>() = second_node.outputs::connect<0>();

  // Compute the dependency graph between nodes
  dataflow::graph graph{&source, &node, &sink, &second_node};

  // Run the dataflow
  dataflow::run_serial(graph);  // Prints 2
  return EXIT_SUCCESS;
}