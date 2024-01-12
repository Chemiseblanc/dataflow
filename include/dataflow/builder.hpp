#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "dataflow/node.hpp"

namespace dataflow {
class factory {
 public:
  explicit factory(std::string type_name) : type{std::move(type_name)} {}

  virtual ~factory() = default;

  [[nodiscard]] virtual std::unique_ptr<node> create(
      const nlohmann::json& config) const = 0;
  [[nodiscard]] virtual nlohmann::json schema() const { return {}; }
  [[nodiscard]] std::string node_type() const;

 private:
  std::string type;
};

class factory_fn final : public factory {
 public:
  using fn_type = std::function<std::unique_ptr<node>(const nlohmann::json&)>;

  factory_fn(const std::string& type_name, fn_type f,
             std::string schema_str = "")
      : factory(type_name),
        fn{std::move(f)},
        schema_string{std::move(schema_str)} {}

  [[nodiscard]] std::unique_ptr<node> create(
      const nlohmann::json& config) const override {
    return fn(config);
  }

  [[nodiscard]] nlohmann::json schema() const override {
    if (!schema_string.empty()) return nlohmann::json::parse(schema_string);
    return nlohmann::json{};
  }

 private:
  fn_type fn;
  std::string schema_string;
};

class registry {
 public:
  static registry& instance();

  static void register_type(std::unique_ptr<factory> f);
  static void register_type(const std::string& type_name, factory_fn::fn_type f,
                            std::string schema_str = "");

  static std::unique_ptr<node> create(const std::string& type,
                                      const nlohmann::json& config);

  static nlohmann::json schema();

  template <typename T>
  static void register_type() {
    register_type(std::make_unique<T>());
  }

  template <typename... Types>
  static void register_types() {
    (register_type<Types>(), ...);
  }

 private:
  registry() = default;

 public:
  registry(const registry&) = delete;

  registry& operator=(const registry&) = delete;

  registry(registry&&) = delete;

  registry& operator=(registry&&) = delete;

 private:
  std::map<std::string, std::unique_ptr<factory>> factories;
};

class builder {
 public:
  explicit builder(const std::string& config_json)
      : builder(std::stringstream(config_json)) {}
  explicit builder(std::istream&& config_reader);

  [[nodiscard]] std::vector<node*> nodes() const;

 private:
  std::map<int, std::unique_ptr<node>> node_map;
};
}  // namespace dataflow
