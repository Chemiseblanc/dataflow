cmake_minimum_required(VERSION 3.25)

if(DEFINED PROJECT_NAME)
    set(IS_SUBPROJECT TRUE)
endif()

project(dataflow
    VERSION 0.1.0
    DESCRIPTION "A C++17 library for dataflow programming"
    LANGUAGES CXX
)

# base library
add_library(dataflow)
target_sources(dataflow
    PRIVATE
        include/dataflow/dataflow.hpp
        include/dataflow/graph.hpp
        include/dataflow/node.hpp
        include/dataflow/runtime.hpp
        src/dataflow.cpp
        src/graph.cpp
        src/node.cpp
)
target_include_directories(dataflow
    PUBLIC
        include
)
target_compile_features(dataflow
    PUBLIC
        cxx_std_17
)
       
# json feature
option(DATAFLOW_JSON "Enable json support" ON)
if (DATAFLOW_JSON)
find_package(nlohmann_json REQUIRED)
target_sources(dataflow
    PRIVATE
        include/dataflow/builder.hpp
        src/builder.cpp
)
target_link_libraries(dataflow
    PUBLIC
        nlohmann_json::nlohmann_json
)
endif()

# taskflow feature
option(DATAFLOW_TASKFLOW "Enable taskflow support" OFF)

# tbb feature
option(DATAFLOW_TBB "Enable Intel-TBB support" OFF)

# hpx feature
option(DATAFLOW_HPX "Enable HPX support" OFF)

#
if (NOT IS_SUBPROJECT)
    option(DATAFLOW_BUILD_EXAMPLES "Build examples" ON)
    if (DATAFLOW_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    include(CTest)
    if (BUILD_TESTS)
        add_subdirectory(tests)
    endif()

    include(GNUInstallDirs)
endif()