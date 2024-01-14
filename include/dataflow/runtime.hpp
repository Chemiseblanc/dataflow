#pragma once

#include "dataflow/api.hpp"
#include "dataflow/graph.hpp"

namespace dataflow {
DATAFLOW_EXPORT void run_serial(graph& g);
}  // namespace dataflow