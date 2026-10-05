# src/search/ -- ALYAN: string matching and scoring.
# Globs, never lists: Alyan adds a .cpp here without touching any shared file.
file(GLOB SEARCH_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT SEARCH_SRC)
  message(FATAL_ERROR "src/search: no sources found by the glob")
endif()

add_library(plag_search STATIC ${SEARCH_SRC})
target_include_directories(plag_search PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_search PUBLIC plag_common plag_index)
