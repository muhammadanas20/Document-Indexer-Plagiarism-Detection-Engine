# Shared contract data types and error codes
file(GLOB CONTRACT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(plag_contract STATIC ${CONTRACT_SRC})
target_include_directories(plag_contract PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_contract PUBLIC plag_common)
