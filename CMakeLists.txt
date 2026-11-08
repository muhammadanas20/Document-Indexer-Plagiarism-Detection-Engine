cmake_minimum_required(VERSION 3.16)
project(document_indexer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

add_library(engine STATIC
  src/algorithms.cpp
  src/documents.cpp
  src/engine.cpp
  src/report.cpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(plag-detect src/main.cpp)
target_link_libraries(plag-detect PRIVATE engine)

enable_testing()
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.cpp)
foreach(test_source ${TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} PRIVATE engine)
  target_compile_definitions(${test_name} PRIVATE
    SAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/samples/students")
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
