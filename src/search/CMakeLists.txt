# Search and scoring module (KMP, candidate generator, scorer, verify mode)
file(GLOB SEARCH_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(plag_search STATIC ${SEARCH_SRC})
target_include_directories(plag_search PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_search PUBLIC plag_common plag_index)
