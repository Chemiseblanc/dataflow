# Dataflow
A C++ library for creating node-based programming models.

## Example
```c++
#include <cstdef>
#include <iostream>

#include "dataflow/node.hpp"

class constant_integer : public dataflow::output<int> {
    explicit constant_integer(const int value) : { output::get<0>() = value; }
};
class multiply_by_two : public dataflow::input<int>, public dataflow::output<int> {
    void operator()() { output::get<0>() = 2 * input::get<0>(); }
};
class print_integer : public dataflow::input<int> {
    void operator()() { std::cout << input::get<0>() << std::endl; }
};

int main(int argc, char* argv[] {
    // Create dataflow objects
    constant_integer source{1};
    multiple_by_two node;
    print_integer sink;
    
    // Connect ports together
    node.inputs::connect<0>() = source.outputs::connect<0>();
    sink.inputs::connect<0>() = node.outputs::connect<0>();
    
    // Compute the dependency graph between nodes
    dataflow::graph graph{&source, &node, &sink};
    
    // Run the dataflow
    dataflow::run_serial(graph); // Prints 2
    return EXIT_SUCCESS;
}

```

## Related
- [dataflow-editor](https://github.com/chemiseblanc/dataflow-editor) A GUI for editing dataflows created using the included json-based node-builder.