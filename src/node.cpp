#include "dataflow/node.hpp"

namespace dataflow {
const std::string& node::label() const { return node_label; }

const std::string& node::input_name(std::size_t i) const {
  return input_ports.at(i)->label;
}
std::size_t node::input_size() const { return std::size(input_ports); }

port& node::input(std::size_t i) { return *input_ports.at(i); }
const port& node::input(std::size_t i) const { return *input_ports.at(i); }


const std::string& node::output_name(std::size_t i) const {
  return output_ports.at(i)->label;
}
std::size_t node::output_size() const { return std::size(output_ports); }

const port& node::output(std::size_t i) const {
  return *output_ports.at(i);
}

port& node::output(std::size_t i) { return *output_ports.at(i);}

bool node::output_connected_to(const node& other) const {
  for (std::size_t i = 0; i < output_size(); ++i) {
    const auto& output_port = output(i);
    for (std::size_t j = 0; j < other.input_size(); ++j) {
      const auto& input_port = other.input(j);
      if (output_port.connected_to(input_port)) return true;
    }
  }
  return false;
}

bool node::input_connected_to(const node& other) const {
  return other.output_connected_to(*this);
}

void node::set_label(const std::string& label) { node_label = label; }

void node::set_input_label(std::size_t i, const std::string& label) {
  input_ports.at(i)->label = label;
}

void node::set_output_label(std::size_t i, const std::string& label) {
  output_ports.at(i)->label = label;
}

std::size_t node::add_inputs(std::vector<port*> ptrs) {
  std::size_t starting_idx = std::size(input_ports);
  input_ports.reserve(std::size(input_ports) + std::size(ptrs));
  for (auto&& ptr : ptrs) {
    input_ports.push_back(std::unique_ptr<port>(ptr));
  }
  return starting_idx;
}

std::size_t node::add_outputs(std::vector<port*> ptrs) {
  std::size_t starting_idx = std::size(output_ports);
  output_ports.reserve(std::size(output_ports) + std::size(ptrs));
  for (auto&& ptr : ptrs) {
    output_ports.push_back(std::unique_ptr<port>(ptr));
  }
  return starting_idx;
}

// std::shared_ptr<void>& node::get_input_data(std::size_t i) {
//   return input_ports.at(i).data;
// }
//
// const std::shared_ptr<void>& node::get_input_data(std::size_t i) const {
//   return input_ports.at(i).data;
// }
//
// std::shared_ptr<void>& node::get_output_data(std::size_t i) {
//   return output_ports.at(i).data;
// }
//
// const std::shared_ptr<void>& node::get_output_data(std::size_t i) const {
//   return output_ports.at(i).data;
// }
}  // namespace dataflow