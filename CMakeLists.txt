# CMakeLists.txt  --  builds the engine (plag-detect) and the tests
# Simple on purpose: one library, one program, one test executable per file.

cmake_minimum_required(VERSION 3.16)
project(document_indexer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra)

# ---- library: every .cpp in src/ except main.cpp --------------------------
file(GLOB ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---- the program ----------------------------------------------------------
add_executable(plag-detect src/main.cpp)
target_link_libraries(plag-detect PRIVATE engine)

# ---- tests: one executable per tests/test_*.cpp ---------------------------
enable_testing()
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.cpp)
foreach(test_source ${TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} PRIVATE engine)
  # the tests read the sample corpus through this path
  target_compile_definitions(${test_name} PRIVATE
    SAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/samples/students")
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
