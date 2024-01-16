# Serialization
add_executable(example_1 ex_1.cpp)
target_link_libraries(example_1 PUBLIC dataflow::dataflow)

# Multi-source inputs
add_executable(example_2 ex_2.cpp)
target_link_libraries(example_2 PUBLIC dataflow::dataflow)