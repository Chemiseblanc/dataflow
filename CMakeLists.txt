cmake_minimum_required(VERSION 3.25)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FetchVcpkg)
fetch_vcpkg(TAG 2023.11.20)

if(DEFINED PROJECT_NAME)
    set(IS_SUBPROJECT TRUE)
endif()

project(Dataflow
    VERSION 0.2.4
    DESCRIPTION "A C++17 library for dataflow programming"
    LANGUAGES CXX
)

include(GenerateExportHeader)

# base library
find_package(nlohmann_json CONFIG REQUIRED)
add_library(dataflow_dataflow)
add_library(dataflow::dataflow ALIAS dataflow_dataflow)
generate_export_header(
    dataflow_dataflow
    BASE_NAME dataflow
    EXPORT_FILE_NAME dataflow/api.hpp
)
set_target_properties(dataflow_dataflow
    PROPERTIES
        EXPORT_NAME dataflow
        OUTPUT_NAME dataflow
)
target_sources(dataflow_dataflow
    INTERFACE
        FILE_SET dataflow_headers
        TYPE HEADERS
        BASE_DIRS include/ ${CMAKE_CURRENT_BINARY_DIR}
        FILES
            include/dataflow/builder.hpp
            include/dataflow/dataflow.hpp
            include/dataflow/graph.hpp
            include/dataflow/node.hpp
            include/dataflow/runtime.hpp
            "${CMAKE_CURRENT_BINARY_DIR}/dataflow/api.hpp"
    PRIVATE
        src/builder.cpp
        src/dataflow.cpp
        src/graph.cpp
        src/node.cpp
        src/runtime.cpp
)
target_include_directories(dataflow_dataflow
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(dataflow_dataflow
    PUBLIC
        cxx_std_17
)
target_link_libraries(dataflow_dataflow
    PUBLIC
        nlohmann_json::nlohmann_json
)

include(GNUInstallDirs)
install(
        TARGETS dataflow_dataflow
        EXPORT DataflowTargets
        FILE_SET dataflow_headers
)
install(
        EXPORT DataflowTargets
        NAMESPACE dataflow::
        DESTINATION
        "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dataflow"
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "cmake/DataflowConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/DataflowConfig.cmake"
        INSTALL_DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dataflow"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
        NO_SET_AND_CHECK_MACRO
)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/DataflowConfigVersion.cmake"
        COMPATIBILITY ExactVersion
)
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/DataflowConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/DataflowConfigVersion.cmake"
        DESTINATION
        "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dataflow"
)

# taskflow feature
option(DATAFLOW_TASKFLOW "Enable taskflow support" OFF)
    if (DATAFLOW_TASKFLOW)
    find_package(taskflow REQUIRED)
    add_library(dataflow_taskflow INTERFACE)
    add_library(dataflow::taskflow ALIAS dataflow_taskflow)
    set_target_properties(dataflow_taskflow
        PROPERTIES
            EXPORT_NAME taskflow
            OUTPUT_NAME taskflow
    )
    target_sources(dataflow_taskflow
        INTERFACE
            FILE_SET dataflow_taskflow_headers
            TYPE HEADERS
            BASE_DIRS include/
            FILES
                include/dataflow/integrations/taskflow.hpp
    )
    target_include_directories(dataflow_taskflow
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(dataflow_taskflow
        INTERFACE
            Taskflow::Taskflow
            dataflow::dataflow
    )

    install(
            TARGETS dataflow_taskflow
            EXPORT DataflowTaskflowTargets
            FILE_SET dataflow_taskflow_headers
    )
    install(
            EXPORT DataflowTaskflowTargets
            NAMESPACE dataflow::
            DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dataflow"
    )
endif()

# tbb feature
option(DATAFLOW_TBB "Enable Intel-TBB support" OFF)
if (DATAFLOW_TBB)

endif()

# hpx feature
option(DATAFLOW_HPX "Enable HPX support" OFF)
if (DATAFLOW_HPX)

endif()

# Tests and examples
if (NOT IS_SUBPROJECT)
    option(DATAFLOW_BUILD_EXAMPLES "Build examples" ON)
    if (DATAFLOW_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()