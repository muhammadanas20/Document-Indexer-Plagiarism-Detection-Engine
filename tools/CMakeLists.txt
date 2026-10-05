# tools/ -- benchmarks and generators, prefixed by owner.
# Owner of this file: Anas (created Week 0). Each of us only ever adds files
# with our own prefix, so nobody shares a filename with anyone.
file(GLOB TOOLS_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(tool_src ${TOOLS_SRC})
  get_filename_component(tool_name ${tool_src} NAME_WE)
  add_executable(${tool_name} ${tool_src})
  set_property(GLOBAL APPEND PROPERTY PLAG_TOOL_TARGETS ${tool_name})
  target_include_directories(${tool_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(${tool_name} PRIVATE plag_contract plag_index)
endforeach()
