#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include "dataflow/api.hpp"

namespace dataflow {

struct DATAFLOW_EXPORT port {
  std::string label;

  virtual ~port() {}
  virtual const std::type_info& type() const = 0;
  virtual void try_connect(const port& other) = 0;
  virtual bool connected_to(const port& other) const = 0;

  port& operator=(const port& other) {
    try_connect(other);
    return *this;
  }
};

class DATAFLOW_EXPORT node {
 public:
  virtual ~node() = default;

  node() {}
  node(const node&) = delete;
  node& operator=(const node&) = delete;

  virtual void operator()() {}

  [[nodiscard]] const std::string& label() const;

  [[nodiscard]] const std::string& input_name(std::size_t i) const;
  [[nodiscard]] std::size_t input_size() const;
  [[nodiscard]] port& input(std::size_t i);
  [[nodiscard]] const port& input(std::size_t i) const;

  [[nodiscard]] const std::string& output_name(std::size_t i) const;
  [[nodiscard]] std::size_t output_size() const;
  [[nodiscard]] const port& output(std::size_t i) const;

  [[nodiscard]] bool output_connected_to(const node& other) const;
  [[nodiscard]] bool input_connected_to(const node& other) const;

  void set_label(const std::string& label);
  void set_input_label(std::size_t i, const std::string& label);
  void set_output_label(std::size_t i, const std::string& label);

 protected:
  std::size_t add_inputs(std::vector<port*> ptrs);
  std::size_t add_outputs(std::vector<port*> ptrs);

  [[nodiscard]] port& output(std::size_t i);

 private:
  std::string node_label;

  std::vector<std::unique_ptr<port>> input_ports;
  std::vector<std::unique_ptr<port>> output_ports;
};

namespace impl {
template <typename T>
struct single_port;

template <typename T>
struct multi_port;

template <typename T>
struct single_port : public port {
  single_port() {}
  // Overload to default-initialize data so it can be assigned to
  explicit single_port(bool) : port_data{new T{}} {}
  [[nodiscard]] const std::type_info& type() const override {
    return typeid(single_port<T>);
  }
  [[nodiscard]] bool empty() const { return port_data == nullptr; }
  [[nodiscard]] bool connected_to(const port& other) const override {
    if (other.type() == type()) {
      return port_data == dynamic_cast<const single_port&>(other).port_data;
    } else if (other.type() == typeid(multi_port<T>)) {
      return dynamic_cast<const multi_port<T>&>(other).connected_to(*this);
    }
    return false;
  }

  void try_connect(const port& other) override {
    if (other.type() == type()) {
      port_data = dynamic_cast<const single_port<T>&>(other).port_data;
    } else {
      throw std::runtime_error("Cannot connect ports of incompatible types");
    }
  }
  const T& data() const {
    if (empty()) {
      throw std::runtime_error("Cannot access data of an empty port");
    }
    return *port_data;
  }
  T& data() { return *port_data; }

  single_port& operator=(const single_port& other) {
    port_data = other.port_data;
    return *this;
  }

  std::shared_ptr<T> port_data;
};

template <typename T>
struct multi_port : public port {
  [[nodiscard]] const std::type_info& type() const override {
    return typeid(multi_port<T>);
  }
  [[nodiscard]] bool empty() const { return port_data.empty(); }
  [[nodiscard]] bool connected_to(const port& other) const override {
    if (other.type() == typeid(single_port<T>)) {
      for (auto& conn : port_data) {
        if (conn == dynamic_cast<const single_port<T>&>(other).port_data) {
          return true;
        }
      }
    }
    return false;
  }

  void try_connect(const port& other) override {
    if (other.type() == typeid(single_port<T>)) {
      port_data.push_back(dynamic_cast<const single_port<T>&>(other).port_data);
    } else {
      throw std::runtime_error(
          "Can only connect a single port of same type to a multi port");
    }
  }
  std::vector<T> data() const {
    std::vector<T> return_data;
    for (auto& conn : port_data) {
      if (conn) {
        return_data.push_back(*conn);
      }
    }
    return return_data;
  }

  multi_port& operator=(const single_port<T>& other) {
    port_data.push_back(other.port_data);
    return *this;
  }

  std::vector<std::shared_ptr<T>> port_data;
};

}  // namespace impl

template <typename T>
struct many {};

template <typename T>
struct port_traits {
  using type = T&;
  using const_type = const T&;
  using port_type = impl::single_port<T>;
};

template <typename T>
struct port_traits<many<T>> {
  using type = std::vector<T>;
  using const_type = const std::vector<T>;
  using port_type = impl::multi_port<T>;
};

template <typename... Inputs>
class inputs : public virtual node {
 public:
  using tuple_type = std::tuple<Inputs...>;
  template <std::size_t i>
  using type = typename port_traits<std::tuple_element_t<i, tuple_type>>::type;
  template <std::size_t i>
  using const_type =
      typename port_traits<std::tuple_element_t<i, tuple_type>>::const_type;
  template <std::size_t i>
  using port_type =
      typename port_traits<std::tuple_element_t<i, tuple_type>>::port_type;

  inputs()
      : starting_idx{node::add_inputs(
            {new typename port_traits<Inputs>::port_type...})} {}

  template <std::size_t i>
  port_type<i>& connect() {
    return dynamic_cast<port_type<i>&>(input(starting_idx + i));
  }

 protected:
  template <std::size_t i>
  const_type<i> get() const {
    return dynamic_cast<const port_type<i>&>(input(starting_idx + i)).data();
  }

  template <std::size_t i>
  [[nodiscard]] bool has() const {
    return dynamic_cast<const port_type<i>&>(input(starting_idx + i)).empty();
  }

 private:
  std::size_t starting_idx;
};

template <typename... Outputs>
class outputs : public virtual node {
 public:
  using tuple_type = std::tuple<Outputs...>;
  template <std::size_t i>
  using type = typename port_traits<std::tuple_element_t<i, tuple_type>>::type;
  template <std::size_t i>
  using port_type =
      typename port_traits<std::tuple_element_t<i, tuple_type>>::port_type;

  outputs()
      : starting_idx{node::add_outputs(
            {new typename port_traits<Outputs>::port_type(true)...})} {}

  template <std::size_t i>
  [[nodiscard]] const port_type<i>& connect() const {
    return dynamic_cast<const port_type<i>&>(output(starting_idx + i));
  }

 protected:
  template <std::size_t i>
  type<i> get() {
    return dynamic_cast<port_type<i>&>(output(starting_idx + i)).data();
  }

 private:
  std::size_t starting_idx;
};
}  // namespace dataflow
