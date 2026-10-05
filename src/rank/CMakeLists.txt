# src/rank/ -- ALYAN: ranking structures (top-k heap, AVL leaderboard).
file(GLOB RANK_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT RANK_SRC)
  message(FATAL_ERROR "src/rank: no sources found by the glob")
endif()

add_library(plag_rank STATIC ${RANK_SRC})
target_include_directories(plag_rank PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_rank PUBLIC plag_common plag_search)
