# tests/ -- one executable per test_*.cpp, one file per class, prefixed by owner.
# Owner: Anas (CMakeLists.txt + harness.hpp). FROZEN after Week 1.
#
# Globs, never lists: Alyan drops test_alyan_kmp.cpp in this folder and it is
# built and registered with ctest without anyone editing this file.
file(GLOB TEST_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

if(NOT TEST_SRC)
  message(FATAL_ERROR "tests/: no test_*.cpp found by the glob")
endif()

# Names are collected so the root `check` target can depend on the real
# targets -- `DEPENDS all` is not addressable in CMake, and without this ctest
# reports "Not Run" for every test on a clean tree.
set_property(GLOBAL PROPERTY PLAG_TEST_TARGETS "")

foreach(test_src ${TEST_SRC})
  get_filename_component(test_name ${test_src} NAME_WE)
  set_property(GLOBAL APPEND PROPERTY PLAG_TEST_TARGETS ${test_name})

  add_executable(${test_name} ${test_src})

  target_include_directories(${test_name} PRIVATE
                             ${PROJECT_SOURCE_DIR}/src
                             ${CMAKE_CURRENT_SOURCE_DIR})

  target_link_libraries(${test_name} PRIVATE
                        plag_contract
                        plag_index
                        plag_search
                        plag_rank
                        plag_report)

  target_compile_definitions(${test_name} PRIVATE
                             PLAG_HARNESS_MAIN
                             PLAG_TEST_SUITE="${test_name}"
                             PLAG_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
                             PLAG_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
                             PLAG_CORPUS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/corpus")

  # Every test gets 60s; the stress test asks for more (Step 8).
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 120)
endforeach()
