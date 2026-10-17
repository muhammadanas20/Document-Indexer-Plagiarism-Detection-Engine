# Ranking module (MaxHeap and AVL RankTree)
file(GLOB RANK_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(plag_rank STATIC ${RANK_SRC})
target_include_directories(plag_rank PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_rank PUBLIC plag_common plag_search)
