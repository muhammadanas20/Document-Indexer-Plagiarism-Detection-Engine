# src/contract/ -- shared types only. Owner: Anas. FROZEN after Week 1.
# The only behaviour here is ErrorCode -> text, so this stays a tiny library.
file(GLOB CONTRACT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(plag_contract STATIC ${CONTRACT_SRC})
target_include_directories(plag_contract PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_contract PUBLIC plag_common)
