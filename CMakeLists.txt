# Top-level CMake build configuration for plag-engine (C++17)
cmake_minimum_required(VERSION 3.20)

project(plag_engine
        VERSION 1.0.0
        DESCRIPTION "Document indexer and plagiarism detection engine"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PLAG_WERROR "Treat compiler warnings as errors" ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_compile_options(-Wall -Wextra -Wconversion)
if(PLAG_WERROR)
  add_compile_options(-Werror)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

add_subdirectory(src/common)
add_subdirectory(src/contract)
add_subdirectory(src/index)
add_subdirectory(src/search)
add_subdirectory(src/rank)
add_subdirectory(src/report)
add_subdirectory(tests)
add_subdirectory(tools)

add_executable(plag-detect src/main.cpp)
target_include_directories(plag-detect PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag-detect PRIVATE
                      plag_contract
                      plag_index
                      plag_search
                      plag_rank
                      plag_report)

get_property(PLAG_TEST_TARGETS GLOBAL PROPERTY PLAG_TEST_TARGETS)
get_property(PLAG_TOOL_TARGETS GLOBAL PROPERTY PLAG_TOOL_TARGETS)

add_custom_target(check
                  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  DEPENDS plag-detect ${PLAG_TEST_TARGETS} ${PLAG_TOOL_TARGETS}
                  COMMENT "Running ctest (plag-engine)")
