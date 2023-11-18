#include <iostream>
#include <string>

#include "dataflow/builder.hpp"
#include "dataflow/graph.hpp"
#include "dataflow/node.hpp"
#include "dataflow/runtime.hpp"

// Node types
class test_source : public dataflow::outputs<int> {
 public:
  test_source(int value) { outputs::get<0>() = value; }
  void operator()() override {
    std::cout << "Created value " << outputs::get<0>() << std::endl;
  }
};

class test_node : public dataflow::inputs<int>, public dataflow::outputs<int> {
 public:
  test_node() {}
  void operator()() override {
    std::cout << "Recieved value: " << inputs::get<0>();
    outputs::get<0>() = 2 * inputs::get<0>();
    std::cout << " Sent value: " << outputs::get<0>() << std::endl;
  }
};

class test_sink : public dataflow::inputs<int> {
 public:
  test_sink() {}
  void operator()() override {
    std::cout << "Recieved value: " << inputs::get<0>() << std::endl;
  }
};

// Node factory objects
// These are required when creating a graph from a json config or otherwise at
// runtime.
// They can optionally define a schema for the node which can be used by other
// applications to generate a UI for creating graphs
class test_source_factory : public dataflow::factory {
 public:
  test_source_factory() : factory("test_source") {}
  std::unique_ptr<dataflow::node> create(
      const nlohmann::json& config) const override {
    return std::make_unique<test_source>(config["value"]);
  }

  nlohmann::json schema() const override {
    using namespace nlohmann::literals;
    return R"(
        {
            "inputs": {},
            "outputs": {
                "0": {
                    "type": "int",
                    "label": "Output"
                }
            },
            "props": {
                "value": {
                    "type": "int",
                    "label": "Value"
                }
            }
        }
    )"_json;
  }
};
class test_node_factory : public dataflow::factory {
 public:
  test_node_factory() : factory("test_node") {}
  std::unique_ptr<dataflow::node> create(
      const nlohmann::json& config) const override {
    return std::make_unique<test_node>();
  }

  nlohmann::json schema() const override {
    using namespace nlohmann::literals;
    return R"(
        {
            "inputs": {
                "0": {
                    "type": "int",
                    "label": "Input"
                }
            },
            "outputs": {
                "0": {
                    "type": "int",
                    "label": "Output"
                }
            },
            "props": {}
        }
    )"_json;
  }
};
class test_sink_factory : public dataflow::factory {
 public:
  test_sink_factory() : factory("test_sink") {}
  std::unique_ptr<dataflow::node> create(
      const nlohmann::json& config) const override {
    return std::make_unique<test_sink>();
  }
  nlohmann::json schema() const override {
    using namespace nlohmann::literals;
    return R"(
        {
            "inputs": {
                "0": {
                    "type": "int",
                    "label": "Input"
                }
            },
            "outputs": {},
            "props": {}
        }
      )"_json;
  }
};

std::string config{R"(
  {
    "nodes": [
    {
      "id": 0,
      "type": "test_source",
      "prop": {
        "value": 1
      }
    },
    {
      "id": 1,
      "type": "test_node",
      "prop": {}
    },
    {
      "id": 2,
      "type": "test_sink",
      "prop": {}
    }
    ],
    "links": [
      {
        "from": {
          "id": 0,
          "port": 0
        },
        "to": {
          "id": 1,
          "port": 0
        }
      },
      {
        "from": {
          "id": 1,
          "port": 0
        },
        "to": {
          "id": 2,
          "port": 0
        }
      }
    ]
  }
)"};

int main(int, char**) {
  dataflow::registry::register_types<test_source_factory, test_node_factory,
                                     test_sink_factory>();
  std::cout << "Registered node schemas: \n";
  std::cout << dataflow::registry::schema().dump(2) << std::endl;

  try {
    dataflow::builder b{config};
    dataflow::graph g{b.nodes()};
    std::cout << "Created graph: \n";
    g.dump(std::cout);
    std::cout << "Running graph: \n";
    dataflow::run_serial(g);

  } catch (std::exception& e) {
    std::cout << "Uncaught Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}