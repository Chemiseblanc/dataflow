#include <gtest/gtest.h>

#include "dataflow/dataflow.hpp"

class IntOut : public dataflow::outputs<int> {};

class FloatIn : public dataflow::inputs<float> {};

TEST(Dataflow, runtime_type_safety) {
  IntOut out;
  FloatIn in;

  EXPECT_ANY_THROW(in.input(0) = std::as_const(out).output(0));
}