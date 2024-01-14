#include "dataflow/builder.hpp"

namespace dataflow {

std::string factory::node_type() const { return type; }

registry& registry::instance() {
  static registry reg;
  return reg;
}

void registry::register_type(std::unique_ptr<factory> factory_ptr) {
  instance().factories[factory_ptr->node_type()] = std::move(factory_ptr);
}

void registry::register_type(const std::string& type_name,
                             factory_fn::fn_type function, std::string schema_str) {
  instance().factories[type_name] =
      std::make_unique<factory_fn>(type_name, function, schema_str);
}

std::unique_ptr<node> registry::create(const std::string& type,
                                       const nlohmann::json& config) {
  auto& inst = instance();
  auto it = inst.factories.find(type);
  if (it == inst.factories.end()) {
    throw std::runtime_error("Unknown node type: " + type);
  }

  auto ptr = it->second->create(config);
  ptr->set_label(type);

  if (auto s = it->second->schema(); !s.is_null()) {
    for (auto&& [key, value] : s["inputs"].items()) {
      ptr->set_input_label(std::stoull(key), value["label"]);
    }
    for (auto&& [key, value] : s["outputs"].items()) {
      ptr->set_output_label(std::stoull(key), value["label"]);
    }
  }
  return ptr;
}

nlohmann::json registry::schema() {
  nlohmann::json result;
  for (auto&& [_, factory] : instance().factories) {
    result[factory->node_type()] = factory->schema();
  }
  return result;
}

builder::builder(std::istream&& config_reader) {
  using namespace nlohmann;

  json config = json::parse(config_reader);
  for (auto&& node : config["nodes"]) {
    int id = node["id"];
    std::string type = node["type"];
    auto& config = node["data"];
    node_map.emplace(id, registry::instance().create(type, config));
  }
  for (auto&& link : config["links"]) {
    int from_id = link["from"]["id"];
    int from_port = link["from"]["port"];
    int to_id = link["to"]["id"];
    int to_port = link["to"]["port"];
    node_map.at(to_id)->input(to_port) =
        node_map.at(from_id)->output(from_port);
  }
}

std::vector<node*> builder::nodes() const {
  std::vector<node*> result;
  for (auto&& [_, ptr] : node_map) {
    result.push_back(ptr.get());
  }
  return result;
}

}  // namespace dataflow