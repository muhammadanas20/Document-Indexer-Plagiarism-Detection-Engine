# src/report/ -- ALYAN: flags, exit codes, the report.
file(GLOB REPORT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT REPORT_SRC)
  message(FATAL_ERROR "src/report: no sources found by the glob")
endif()

add_library(plag_report STATIC ${REPORT_SRC})
target_include_directories(plag_report PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(plag_report PUBLIC plag_common plag_index plag_search plag_rank)
