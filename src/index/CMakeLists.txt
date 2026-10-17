# Indexing module (file reading, tokenizer, rolling hash, hash tables, index builder, span merge)
file(GLOB INDEX_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(plag_index STATIC ${INDEX_SRC})
target_include_directories(plag_index PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_index PUBLIC plag_common)
