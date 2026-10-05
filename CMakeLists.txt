# ============================================================================
# plag-engine -- Document Indexer & Plagiarism Detection Engine (C++17)
#
# TOP-LEVEL BUILD FILE.  Owner: Anas.  Written once in Week 0 and then FROZEN.
# Never edit this file again to add a source file: each area's own
# CMakeLists.txt picks sources up with file(GLOB ...), which is the single
# decision that removes ~90% of the merge friction in a two-person repo.
#
# If you genuinely need something changed here, open an issue titled
# "contract: need <hook>" -- Anas adds it in a 5-line commit nobody clashes on.
# ============================================================================
cmake_minimum_required(VERSION 3.20)

project(plag_engine
        VERSION 0.1.0
        DESCRIPTION "Document indexer and plagiarism detection engine"
        LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global rules: C++17, no compiler extensions, warnings on, warnings are errors.
# "Clean build only" -- -Wall -Wextra -Wconversion with zero warnings.
# ---------------------------------------------------------------------------
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

# Export compile_commands.json for editors / clang-tidy. Git-ignored.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

# ---------------------------------------------------------------------------
# One add_subdirectory per area. Order matters only for the target definitions
# that later areas link against.
# ---------------------------------------------------------------------------
add_subdirectory(src/common)     # shared tools            -- Anas, frozen
add_subdirectory(src/contract)   # shared types            -- Anas, frozen
add_subdirectory(src/index)      # reading + indexing      -- ANAS
add_subdirectory(src/search)     # matching + scoring      -- ALYAN
add_subdirectory(src/rank)       # heap + AVL leaderboard  -- ALYAN
add_subdirectory(src/report)     # CLI + report            -- ALYAN
add_subdirectory(tests)          # globs test_*.cpp
add_subdirectory(tools)          # benchmarks + generators

# ---------------------------------------------------------------------------
# The entry point. Owned by Anas; command wiring lands in whichever week
# docs/tasks.md lists main.cpp.
# ---------------------------------------------------------------------------
add_executable(plag-detect src/main.cpp)
target_include_directories(plag-detect PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag-detect PRIVATE
                      plag_contract
                      plag_index
                      plag_search
                      plag_rank
                      plag_report)

# `make check` -- what CI runs on every push and PR.
# Depends on the real test targets (collected by tests/CMakeLists.txt) so a
# clean tree builds before ctest runs; otherwise every test reports "Not Run".
get_property(PLAG_TEST_TARGETS GLOBAL PROPERTY PLAG_TEST_TARGETS)
get_property(PLAG_TOOL_TARGETS GLOBAL PROPERTY PLAG_TOOL_TARGETS)

add_custom_target(check
                  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  DEPENDS plag-detect ${PLAG_TEST_TARGETS} ${PLAG_TOOL_TARGETS}
                  COMMENT "Running ctest (plag-engine)")
