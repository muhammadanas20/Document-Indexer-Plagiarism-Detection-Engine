# src/index/ -- Anas: reading, tokenising, indexing.
# Globs, never lists: adding a .cpp here never touches a shared file.
file(GLOB INDEX_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT INDEX_SRC)
  message(FATAL_ERROR "src/index: no sources found by the glob")
endif()

add_library(plag_index STATIC ${INDEX_SRC})
target_include_directories(plag_index PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_index PUBLIC plag_common)
set_target_properties(plag_index PROPERTIES POSITION_INDEPENDENT_CODE OFF)
