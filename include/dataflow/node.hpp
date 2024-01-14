#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "dataflow/api.hpp"

namespace dataflow {
class DATAFLOW_EXPORT node {
 public:
  virtual ~node() = default;

  virtual void operator()() {}

  [[nodiscard]] const std::string& label() const;

  [[nodiscard]] const std::string& input_name(std::size_t i) const;
  [[nodiscard]] std::size_t input_size() const;
  [[nodiscard]] std::shared_ptr<void>& input(std::size_t i);

  [[nodiscard]] const std::string& output_name(std::size_t i) const;
  [[nodiscard]] std::size_t output_size() const;
  [[nodiscard]] const std::shared_ptr<void>& output(std::size_t i) const;

  [[nodiscard]] bool output_connected_to(const node& other) const;
  [[nodiscard]] bool input_connected_to(const node& other) const;

  void set_label(const std::string& label);
  void set_input_label(std::size_t i, const std::string& label);
  void set_output_label(std::size_t i, const std::string& label);

 protected:
  std::size_t add_inputs(std::vector<std::shared_ptr<void>> ptrs);
  std::size_t add_outputs(std::vector<std::shared_ptr<void>> ptrs);

  std::shared_ptr<void>& get_input_data(std::size_t i);
  [[nodiscard]] const std::shared_ptr<void>& get_input_data(
      std::size_t i) const;

  std::shared_ptr<void>& get_output_data(std::size_t i);
  [[nodiscard]] const std::shared_ptr<void>& get_output_data(
      std::size_t i) const;

 private:
  std::string node_label;

  struct port {
    std::string label;
    std::shared_ptr<void> data;
  };

  std::vector<port> input_ports;
  std::vector<port> output_ports;
};

template <typename... Inputs>
class inputs : public virtual node {
 public:
  using tuple_type = std::tuple<Inputs...>;
  template <std::size_t i>
  using type = std::tuple_element_t<i, tuple_type>;

 public:
  inputs() { starting_idx = node::add_inputs({std::shared_ptr<Inputs>{}...}); }

  template <std::size_t i>
  const type<i>& get() const {
    auto ptr =
        std::static_pointer_cast<type<i>>(get_input_data(starting_idx + i));
    if (ptr == nullptr) throw std::runtime_error("Invalid input type");
    return *ptr;
  }

  template <std::size_t i>
  [[nodiscard]] bool has() const {
    auto ptr =
        std::static_pointer_cast<type<i>>(get_input_data(starting_idx + i));
    return ptr != nullptr;
  }

  std::shared_ptr<void>& connect(const std::size_t i) {
    return get_input_data(starting_idx + i);
  }

 private:
  std::size_t starting_idx;
};

template <typename... Outputs>
class outputs : public virtual node {
 public:
  using tuple_type = std::tuple<Outputs...>;
  template <std::size_t i>
  using type = std::tuple_element_t<i, tuple_type>;

 public:
  outputs() {
    starting_idx = node::add_outputs({std::make_shared<Outputs>()...});
  }

  template <std::size_t i>
  type<i>& get() {
    auto ptr =
        std::static_pointer_cast<type<i>>(get_output_data(starting_idx + i));
    if (ptr == nullptr) throw std::runtime_error("Invalid output type");
    return *ptr;
  }

  [[nodiscard]] const std::shared_ptr<void>& connect(
      const std::size_t i) const {
    return get_output_data(starting_idx + i);
  }

 private:
  std::size_t starting_idx;
};
}  // namespace dataflow
