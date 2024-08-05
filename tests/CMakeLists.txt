find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(dataflow_test)
gtest_discover_tests(dataflow_test)

target_include_directories(dataflow_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(dataflow_test
    PRIVATE
        GTest::gtest
        GTest::gtest_main

        dataflow::dataflow
)

target_sources(dataflow_test
    PRIVATE
        type_safety.cpp
)
